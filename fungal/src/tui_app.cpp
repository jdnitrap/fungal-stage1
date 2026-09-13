#include "hardware.hpp"
#include "substrate.hpp"
#include "autonomous_node.hpp"
#include "self_model.hpp"
#include "constraints.hpp"
#include "alignment.hpp"
#include "monitor.hpp"
#include "learning.hpp"
#include "self_improvement.hpp"
#include "neural_integration.hpp"
#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <sstream>
#include <thread>
#include <chrono>
#include <iomanip>
#include <cmath>

using namespace fungal;

// ANSI color codes for enhanced UI
class Color {
public:
    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* BOLD = "\033[1m";
    static constexpr const char* GREEN = "\033[32m";
    static constexpr const char* YELLOW = "\033[33m";
    static constexpr const char* BLUE = "\033[34m";
    static constexpr const char* CYAN = "\033[36m";
    static constexpr const char* RED = "\033[31m";
    static constexpr const char* MAGENTA = "\033[35m";
};

class FungalTUI {
private:
    HardwareInfo hardware_;
    std::shared_ptr<Substrate> substrate_;
    std::map<std::string, std::unique_ptr<AutonomousNode>> nodes_;
    std::map<std::string, std::unique_ptr<SelfAwareness>> awareness_;
    std::map<std::string, std::unique_ptr<Monitor>> monitors_;
    std::unique_ptr<ConstraintEngine> constraints_;
    std::unique_ptr<ValueAlignment> alignment_;
    std::unique_ptr<AdaptiveLearning> learning_;
    std::unique_ptr<ReinforcementLoop> rl_;
    std::unique_ptr<StrategyGenerator> strategy_gen_;
    std::shared_ptr<NeuralEmbedding> neural_;

    std::string selected_node_ = "";
    std::vector<std::string> node_list_;
    std::string status_message_ = "Ready";
    int cycle_count_ = 0;

public:
    FungalTUI() {
        hardware_ = HardwareDetector::detect();
        substrate_ = std::make_shared<Substrate>();
        constraints_ = std::make_unique<ConstraintEngine>();
        alignment_ = std::make_unique<ValueAlignment>();
        learning_ = std::make_unique<AdaptiveLearning>();
        rl_ = std::make_unique<ReinforcementLoop>();
        strategy_gen_ = std::make_unique<StrategyGenerator>();
        neural_ = std::make_shared<NeuralEmbedding>(8, 4, 1);

        constraints_->enforce_energy_limit(100.0);
        constraints_->set_strategy_whitelist({"verify", "analyze", "learn"});

        status_message_ = "Fungal System initialized. Ready for interaction.";
    }

    void create_node(const std::string& node_id, const std::string& goal) {
        substrate_->register_node(node_id);
        auto node = std::make_unique<AutonomousNode>(node_id, substrate_);
        node->initialize();
        node->set_goal(goal);

        auto aware = std::make_unique<SelfAwareness>(node_id, hardware_);
        aware->set_primary_goal(goal);
        aware->add_strategy("verify");
        aware->add_strategy("analyze");
        aware->add_strategy("learn");
        aware->set_energy_state(90.0, 100.0);

        auto monitor = std::make_unique<Monitor>(node_id);

        nodes_[node_id] = std::move(node);
        awareness_[node_id] = std::move(aware);
        monitors_[node_id] = std::move(monitor);
        node_list_.push_back(node_id);
        selected_node_ = node_id;

        status_message_ = "Created node: " + node_id + " with goal: " + goal;
    }

    void run_cycle(const std::string& node_id) {
        if (nodes_.find(node_id) == nodes_.end()) {
            status_message_ = "Node not found: " + node_id;
            return;
        }

        nodes_[node_id]->process_cycle();
        awareness_[node_id]->record_decision("cycle", true);
        monitors_[node_id]->log_decision("cycle", "Completed process cycle", 1.5);
        cycle_count_++;
        status_message_ = "Completed cycle for: " + node_id;
    }

    void send_signal(const std::string& from_node, const std::string& content) {
        Signal sig{"test_signal", from_node, content, 0.0, 0.8};
        substrate_->broadcast_signal(sig);
        status_message_ = "Signal sent from " + from_node + ": " + content;
    }

    void record_learning(const std::string& source, bool correct) {
        learning_->record_source_outcome(source, correct);
        if (correct) {
            rl_->observe_reward("verify", 10.0);
        } else {
            rl_->observe_penalty("verify", 5.0);
        }
        status_message_ = "Recorded learning: " + source + " -> " + (correct ? "correct" : "incorrect");
    }

    void add_strategy(const std::string& node_id, const std::string& strategy) {
        if (awareness_.find(node_id) != awareness_.end()) {
            awareness_[node_id]->add_strategy(strategy);
            status_message_ = "Added strategy: " + strategy + " to " + node_id;
        }
    }

    void remove_strategy(const std::string& node_id, const std::string& strategy) {
        if (awareness_.find(node_id) != awareness_.end()) {
            awareness_[node_id]->remove_strategy(strategy);
            status_message_ = "Removed strategy: " + strategy + " from " + node_id;
        }
    }

    std::string get_node_status(const std::string& node_id) {
        if (awareness_.find(node_id) == awareness_.end()) {
            return "Node not found";
        }

        std::stringstream ss;
        auto& aware = awareness_[node_id];

        ss << Color::BOLD << "═══ Node: " << node_id << " ═══" << Color::RESET << "\n";
        ss << Color::CYAN << "Goal:" << Color::RESET << " " << aware->get_primary_goal() << "\n";
        ss << Color::CYAN << "Energy:" << Color::RESET << " " << Color::YELLOW << aware->get_energy_status() << Color::RESET << "\n";

        double energy_pct = (aware->get_success_rate() * 100.0);
        ss << "Energy Bar: [";
        int bar_length = 20;
        int filled = static_cast<int>(energy_pct / 5.0);
        for (int i = 0; i < bar_length; ++i) {
            if (i < filled) ss << Color::GREEN << "█" << Color::RESET;
            else ss << "░";
        }
        ss << "] " << std::fixed << std::setprecision(1) << energy_pct << "%\n";

        ss << Color::CYAN << "Strategies:" << Color::RESET << " " << aware->get_active_strategies().size() << "\n";
        for (const auto& strat : aware->get_active_strategies()) {
            ss << "  • " << strat << "\n";
        }

        ss << Color::CYAN << "Success Rate:" << Color::RESET << " " << std::fixed << std::setprecision(2)
           << (aware->get_success_rate() * 100.0) << "%\n";
        ss << Color::CYAN << "Best Strategy:" << Color::RESET << " " << aware->get_best_performing_strategy() << "\n";

        return ss.str();
    }

    std::string get_audit_trail(const std::string& node_id) {
        if (monitors_.find(node_id) == monitors_.end()) {
            return "No audit trail for node: " + node_id;
        }

        std::stringstream ss;
        ss << Color::BOLD << "═══ Audit Trail: " << node_id << " ═══" << Color::RESET << "\n";
        ss << monitors_[node_id]->generate_audit_report();
        return ss.str();
    }

    std::string get_constraint_status() {
        std::stringstream ss;
        ss << Color::BOLD << "═══ Safety Constraints ═══" << Color::RESET << "\n";
        ss << constraints_->describe_constraints();
        return ss.str();
    }

    std::string get_alignment_status() {
        std::stringstream ss;
        ss << Color::BOLD << "═══ Value Alignment ═══" << Color::RESET << "\n";
        ss << alignment_->describe_alignment();
        return ss.str();
    }

    std::string get_system_status() {
        std::stringstream ss;
        ss << Color::BOLD << "═══ Fungal System Status ═══" << Color::RESET << "\n";
        ss << Color::CYAN << "Hardware:" << Color::RESET << " " << hardware_.cpu_cores << " cores, "
           << hardware_.total_memory_bytes / (1024*1024*1024) << "GB RAM (" << hardware_.arch << ")\n";
        ss << Color::CYAN << "Nodes:" << Color::RESET << " " << nodes_.size() << "\n";
        ss << Color::CYAN << "Total Cycles:" << Color::RESET << " " << cycle_count_ << "\n";
        ss << Color::CYAN << "Status:" << Color::RESET << " " << Color::GREEN << status_message_ << Color::RESET << "\n";

        if (!nodes_.empty()) {
            ss << Color::CYAN << "Active Nodes:" << Color::RESET << "\n";
            for (const auto& node_id : node_list_) {
                double energy = 0.0;
                if (awareness_.find(node_id) != awareness_.end()) {
                    energy = awareness_[node_id]->get_success_rate();
                }
                ss << "  • " << node_id << " [" << std::fixed << std::setprecision(1) << (energy*100.0) << "%]\n";
            }
        }

        return ss.str();
    }

    std::string get_learning_status() {
        std::stringstream ss;
        ss << Color::BOLD << "═══ Learning & Strategies ═══" << Color::RESET << "\n";
        ss << learning_->describe_learned_trusts();
        ss << "\n";
        ss << rl_->describe_learned_strategies();
        return ss.str();
    }

    std::string get_neural_status() {
        std::stringstream ss;
        ss << Color::BOLD << "═══ Neural Network ═══" << Color::RESET << "\n";
        ss << neural_->describe_network();

        // Simulate network output visualization
        std::vector<double> test_input(8, 0.5);
        auto output = neural_->forward_pass(test_input);

        ss << "\nNetwork Output Visualization:\n";
        ss << "Input: [";
        for (size_t i = 0; i < test_input.size(); ++i) {
            ss << std::fixed << std::setprecision(2) << test_input[i];
            if (i < test_input.size()-1) ss << ", ";
        }
        ss << "]\n";

        ss << "Output: " << Color::MAGENTA;
        double out_val = output[0];
        ss << std::fixed << std::setprecision(3) << out_val << Color::RESET << "\n";

        // Visual representation
        ss << "Output Bar: [";
        int bar_len = 30;
        int filled = static_cast<int>(out_val * bar_len);
        for (int i = 0; i < bar_len; ++i) {
            if (i < filled) ss << Color::MAGENTA << "█" << Color::RESET;
            else ss << "░";
        }
        ss << "]\n";

        return ss.str();
    }

    std::vector<std::string> get_node_list() const {
        return node_list_;
    }

    void set_selected_node(const std::string& node_id) {
        selected_node_ = node_id;
    }

    std::string get_selected_node() const {
        return selected_node_;
    }
};

class TerminalUI {
private:
    FungalTUI& system_;
    bool running_ = true;

public:
    TerminalUI(FungalTUI& system) : system_(system) {}

    void clear_screen() {
        system("clear");
    }

    void display_header() {
        clear_screen();
        std::cout << "\n";
        std::cout << Color::BOLD << Color::CYAN;
        std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
        std::cout << "║          FUNGAL SYSTEM - INTERACTIVE TERMINAL UI                ║\n";
        std::cout << "║     Self-Aware Autonomous AI - Truth Seeking Network             ║\n";
        std::cout << "║     Enhanced Edition with Neural & Audit Capabilities           ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
        std::cout << Color::RESET << "\n";
    }

    void display_main_menu() {
        display_header();
        std::cout << Color::BOLD << "Main Menu:" << Color::RESET << "\n";
        std::cout << "  " << Color::GREEN << "[1]" << Color::RESET << " Create Node\n";
        std::cout << "  " << Color::GREEN << "[2]" << Color::RESET << " Node Management\n";
        std::cout << "  " << Color::GREEN << "[3]" << Color::RESET << " Send Signals\n";
        std::cout << "  " << Color::GREEN << "[4]" << Color::RESET << " Learning & Strategies\n";
        std::cout << "  " << Color::GREEN << "[5]" << Color::RESET << " System Status\n";
        std::cout << "  " << Color::GREEN << "[6]" << Color::RESET << " Audit Trail\n";
        std::cout << "  " << Color::GREEN << "[7]" << Color::RESET << " Safety & Constraints\n";
        std::cout << "  " << Color::GREEN << "[8]" << Color::RESET << " Neural Network\n";
        std::cout << "  " << Color::GREEN << "[9]" << Color::RESET << " Strategy Management\n";
        std::cout << "  " << Color::GREEN << "[0]" << Color::RESET << " Exit\n\n";
        std::cout << "Enter choice: ";
    }

    void create_node_dialog() {
        display_header();
        std::string node_id, goal;

        std::cout << Color::BOLD << "Create New Node" << Color::RESET << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        std::cout << "Node ID (e.g., node_1): ";
        std::getline(std::cin, node_id);

        std::cout << "Goal (e.g., verify_claims, analyze_code): ";
        std::getline(std::cin, goal);

        system_.create_node(node_id, goal);
        std::cout << "\n" << Color::GREEN << "✓ Node created successfully!" << Color::RESET << "\n";
        std::cout << "Press Enter to continue...";
        std::cin.get();
    }

    void node_management_menu() {
        while (true) {
            display_header();
            auto nodes = system_.get_node_list();

            if (nodes.empty()) {
                std::cout << Color::YELLOW << "No nodes created yet. Create one from the main menu." << Color::RESET << "\n";
                std::cout << "Press Enter to return...";
                std::cin.get();
                return;
            }

            std::cout << Color::BOLD << "Node Management" << Color::RESET << "\n";
            std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

            std::cout << "Available Nodes:\n";
            for (size_t i = 0; i < nodes.size(); ++i) {
                std::cout << "  " << Color::GREEN << "[" << (i+1) << "]" << Color::RESET << " " << nodes[i] << "\n";
            }
            std::cout << "  " << Color::RED << "[0]" << Color::RESET << " Back\n\n";
            std::cout << "Select node: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 0) return;
            if (choice > 0 && choice <= static_cast<int>(nodes.size())) {
                system_.set_selected_node(nodes[choice-1]);
                show_node_details(nodes[choice-1]);
            }
        }
    }

    void show_node_details(const std::string& node_id) {
        while (true) {
            display_header();
            std::cout << system_.get_node_status(node_id);
            std::cout << "\n" << Color::BOLD << "Options:" << Color::RESET << "\n";
            std::cout << "  " << Color::GREEN << "[1]" << Color::RESET << " Run Cycle\n";
            std::cout << "  " << Color::GREEN << "[2]" << Color::RESET << " Send Signal\n";
            std::cout << "  " << Color::GREEN << "[3]" << Color::RESET << " Record Learning\n";
            std::cout << "  " << Color::GREEN << "[4]" << Color::RESET << " View Audit Trail\n";
            std::cout << "  " << Color::RED << "[0]" << Color::RESET << " Back\n\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 0) return;

            switch (choice) {
                case 1:
                    system_.run_cycle(node_id);
                    std::cout << "\n" << Color::GREEN << "✓ Cycle completed!" << Color::RESET << "\n";
                    break;
                case 2: {
                    std::cout << "\nSignal content: ";
                    std::string content;
                    std::getline(std::cin, content);
                    system_.send_signal(node_id, content);
                    std::cout << Color::GREEN << "✓ Signal sent!" << Color::RESET << "\n";
                    break;
                }
                case 3: {
                    std::cout << "\nOutcome (1=correct, 0=incorrect): ";
                    int outcome;
                    std::cin >> outcome;
                    std::cin.ignore();
                    system_.record_learning(node_id, outcome == 1);
                    std::cout << Color::GREEN << "✓ Learning recorded!" << Color::RESET << "\n";
                    break;
                }
                case 4: {
                    display_header();
                    std::cout << system_.get_audit_trail(node_id);
                    std::cout << "\nPress Enter to return...";
                    std::cin.get();
                    continue;
                }
            }

            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    }

    void signals_menu() {
        display_header();
        auto nodes = system_.get_node_list();

        if (nodes.empty()) {
            std::cout << Color::YELLOW << "No nodes created yet." << Color::RESET << "\n";
            std::cout << "Press Enter to return...";
            std::cin.get();
            return;
        }

        std::cout << Color::BOLD << "Send Signal" << Color::RESET << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        std::cout << "From Node:\n";
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::cout << "  " << Color::GREEN << "[" << (i+1) << "]" << Color::RESET << " " << nodes[i] << "\n";
        }
        std::cout << "\nSelect: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice > 0 && choice <= static_cast<int>(nodes.size())) {
            std::string from_node = nodes[choice-1];
            std::cout << "\nSignal content: ";
            std::string content;
            std::getline(std::cin, content);

            system_.send_signal(from_node, content);
            std::cout << "\n" << Color::GREEN << "✓ Signal sent!" << Color::RESET << "\n";
        }

        std::cout << "Press Enter to return...";
        std::cin.get();
    }

    void learning_menu() {
        display_header();
        std::cout << system_.get_learning_status();

        std::cout << "\n" << Color::BOLD << "Options:" << Color::RESET << "\n";
        std::cout << "  " << Color::GREEN << "[1]" << Color::RESET << " Record Success\n";
        std::cout << "  " << Color::GREEN << "[2]" << Color::RESET << " Record Failure\n";
        std::cout << "  " << Color::RED << "[0]" << Color::RESET << " Back\n\n";
        std::cout << "Choice: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice == 1 || choice == 2) {
            std::cout << "Source name: ";
            std::string source;
            std::getline(std::cin, source);
            system_.record_learning(source, choice == 1);
            std::cout << "\n" << Color::GREEN << "✓ Learning recorded!" << Color::RESET << "\n";
            std::cout << "Press Enter to continue...";
            std::cin.get();
        }
    }

    void system_status_menu() {
        display_header();
        std::cout << system_.get_system_status();
        std::cout << "\nPress Enter to return...";
        std::cin.get();
    }

    void audit_trail_menu() {
        display_header();
        auto nodes = system_.get_node_list();

        if (nodes.empty()) {
            std::cout << Color::YELLOW << "No nodes created yet." << Color::RESET << "\n";
            std::cout << "Press Enter to return...";
            std::cin.get();
            return;
        }

        std::cout << Color::BOLD << "Audit Trail" << Color::RESET << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        std::cout << "Select Node:\n";
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::cout << "  " << Color::GREEN << "[" << (i+1) << "]" << Color::RESET << " " << nodes[i] << "\n";
        }
        std::cout << "\nSelect: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice > 0 && choice <= static_cast<int>(nodes.size())) {
            display_header();
            std::cout << system_.get_audit_trail(nodes[choice-1]);
            std::cout << "\nPress Enter to return...";
            std::cin.get();
        }
    }

    void constraints_menu() {
        display_header();
        std::cout << system_.get_constraint_status();
        std::cout << "\n";
        std::cout << system_.get_alignment_status();
        std::cout << "\nPress Enter to return...";
        std::cin.get();
    }

    void neural_menu() {
        display_header();
        std::cout << system_.get_neural_status();
        std::cout << "\nPress Enter to return...";
        std::cin.get();
    }

    void strategy_menu() {
        display_header();
        auto nodes = system_.get_node_list();

        if (nodes.empty()) {
            std::cout << Color::YELLOW << "No nodes created yet." << Color::RESET << "\n";
            std::cout << "Press Enter to return...";
            std::cin.get();
            return;
        }

        std::cout << Color::BOLD << "Strategy Management" << Color::RESET << "\n";
        std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n";

        std::cout << "Select Node:\n";
        for (size_t i = 0; i < nodes.size(); ++i) {
            std::cout << "  " << Color::GREEN << "[" << (i+1) << "]" << Color::RESET << " " << nodes[i] << "\n";
        }
        std::cout << "\nSelect: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore();

        if (choice > 0 && choice <= static_cast<int>(nodes.size())) {
            manage_strategies(nodes[choice-1]);
        }
    }

    void manage_strategies(const std::string& node_id) {
        while (true) {
            display_header();
            std::cout << Color::BOLD << "Strategies for " << node_id << Color::RESET << "\n";
            std::cout << "  " << Color::GREEN << "[1]" << Color::RESET << " Add Strategy\n";
            std::cout << "  " << Color::GREEN << "[2]" << Color::RESET << " Remove Strategy\n";
            std::cout << "  " << Color::GREEN << "[3]" << Color::RESET << " View Strategies\n";
            std::cout << "  " << Color::RED << "[0]" << Color::RESET << " Back\n\n";
            std::cout << "Choice: ";

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            if (choice == 0) return;

            switch (choice) {
                case 1: {
                    std::cout << "\nStrategy name (verify, analyze, learn, integrate): ";
                    std::string strategy;
                    std::getline(std::cin, strategy);
                    system_.add_strategy(node_id, strategy);
                    std::cout << Color::GREEN << "✓ Strategy added!" << Color::RESET << "\n";
                    std::cout << "Press Enter to continue...";
                    std::cin.get();
                    break;
                }
                case 2: {
                    std::cout << "\nStrategy name to remove: ";
                    std::string strategy;
                    std::getline(std::cin, strategy);
                    system_.remove_strategy(node_id, strategy);
                    std::cout << Color::GREEN << "✓ Strategy removed!" << Color::RESET << "\n";
                    std::cout << "Press Enter to continue...";
                    std::cin.get();
                    break;
                }
                case 3: {
                    display_header();
                    std::cout << system_.get_node_status(node_id);
                    std::cout << "\nPress Enter to return...";
                    std::cin.get();
                    break;
                }
            }
        }
    }

    void run() {
        while (running_) {
            display_main_menu();

            int choice;
            std::cin >> choice;
            std::cin.ignore();

            switch (choice) {
                case 1:
                    create_node_dialog();
                    break;
                case 2:
                    node_management_menu();
                    break;
                case 3:
                    signals_menu();
                    break;
                case 4:
                    learning_menu();
                    break;
                case 5:
                    system_status_menu();
                    break;
                case 6:
                    audit_trail_menu();
                    break;
                case 7:
                    constraints_menu();
                    break;
                case 8:
                    neural_menu();
                    break;
                case 9:
                    strategy_menu();
                    break;
                case 0:
                    running_ = false;
                    break;
                default:
                    std::cout << Color::RED << "Invalid choice." << Color::RESET << "\n";
                    std::cout << "Press Enter to continue...";
                    std::cin.get();
            }
        }

        display_header();
        std::cout << Color::BOLD << Color::CYAN << "Fungal System TUI - Shutting down..." << Color::RESET << "\n";
        std::cout << "Thank you for using the Fungal System!\n\n";
    }
};

int main() {
    FungalTUI system;
    TerminalUI ui(system);
    ui.run();

    return 0;
}
