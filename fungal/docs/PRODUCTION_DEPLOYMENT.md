# Fungal Production System Deployment Guide

## Overview

This guide covers deploying the Fungal Production System in enterprise environments. The system provides complete infrastructure for running the autonomous AI system with safety constraints, clustering support, monitoring, and disaster recovery.

## System Architecture

The production system consists of 12 integrated modules:

### Core Modules
- **ConfigManager**: Centralized configuration management
- **Logger**: Structured logging with spdlog
- **Persistence**: State management and snapshots
- **APIServer**: HTTP REST API for all operations

### Operational Modules
- **Metrics**: Performance monitoring and Prometheus export
- **HealthCheck**: System health monitoring and alerts
- **ThreadPool**: Concurrent task execution
- **Security**: Authentication, authorization, and encryption

### Infrastructure Modules
- **Database**: Query execution, transactions, persistence
- **ClusterManager**: Distributed deployment with consensus
- **BackupManager**: Backup/recovery and point-in-time restore
- **ProductionSystem**: Master coordinator for all components

## Installation

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+)
- CMake 3.10+
- Linux, macOS, or Windows

### Build

```bash
cd fungal
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j4
```

### Installation Targets

Three executable entry points are available:

1. **fungal-server** - Production server
2. **fungal-cli** - Management CLI tool
3. **fungal-dashboard** - Web dashboard backend

## Configuration

### Quick Start

1. Copy configuration template:
```bash
cp config.yaml.example config.yaml
```

2. Edit `config.yaml` for your environment:
```yaml
api:
  port: 8080
  host: 0.0.0.0

logging:
  level: info
  directory: logs

database:
  type: sqlite
  file: fungal.db
```

3. Start the server:
```bash
./fungal-server -c config.yaml
```

### Configuration Sections

**API Server**
```yaml
api:
  port: 8080
  host: 0.0.0.0
  enable_cors: true
  request_timeout_seconds: 30
```

**Logging**
```yaml
logging:
  level: info  # trace, debug, info, warn, error, critical
  directory: logs
  console_output: true
  max_file_size_mb: 10
  max_files: 3
```

**Database**
```yaml
database:
  type: sqlite  # sqlite, postgresql, mysql
  file: fungal.db  # for SQLite
```

**Threading**
```yaml
threading:
  num_threads: 4
  task_queue_size: 1000
```

**Security**
```yaml
security:
  auth_method: 1  # 0=none, 1=token, 2=basic, 3=certificate
  enable_https: false
  token_expiry_hours: 24
```

**Clustering**
```yaml
clustering:
  enabled: false
  cluster_id: default
  node_id: node1
  peer_addresses:
    - node2:9001
    - node3:9001
```

**Backup**
```yaml
backup:
  enabled: true
  directory: backups/
  retention_days: 30
  schedule:
    enabled: true
    hour: 2
    minute: 0
    type: incremental
```

## Running the Server

### Standalone Mode

```bash
./fungal-server -c config.yaml
```

The server will:
1. Initialize all components
2. Start the API server on the configured port
3. Register built-in health/status endpoints
4. Begin monitoring system health
5. Accept requests for management operations

### Cluster Mode

Enable clustering in configuration:

```yaml
clustering:
  enabled: true
  cluster_id: production-cluster
  node_id: node1
  peer_addresses:
    - node2.example.com:9001
    - node3.example.com:9001
```

Start multiple nodes:
```bash
# Node 1
./fungal-server -c config-node1.yaml

# Node 2
./fungal-server -c config-node2.yaml

# Node 3
./fungal-server -c config-node3.yaml
```

The system will:
- Auto-discover peer nodes
- Elect a leader via Raft consensus
- Replicate state across the cluster
- Handle node failures gracefully

## Management Operations

### CLI Tool

```bash
# System status
./fungal-cli status

# Show configuration
./fungal-cli config

# Get specific config value
./fungal-cli config get api.port

# Set configuration value
./fungal-cli config set api.port 9000

# System metrics
./fungal-cli metrics

# Health status
./fungal-cli health

# Backup operations
./fungal-cli backup list
./fungal-cli backup create
./fungal-cli backup restore <backup_id>

# Security management
./fungal-cli security
./fungal-cli security add-user admin password123

# Cluster status
./fungal-cli cluster status
```

### REST API

All operations are also available via HTTP API:

```bash
# Health check
curl http://localhost:8080/health

# System status
curl http://localhost:8080/status

# Metrics
curl http://localhost:8080/metrics

# Create backup
curl -X POST http://localhost:8080/api/backups/create

# Restore backup
curl -X POST http://localhost:8080/api/backups/restore \
  -H "Content-Type: application/json" \
  -d '{"backup_id": "backup_123"}'
```

## Monitoring

### Built-in Health Checks

The system automatically monitors:
- Memory usage (default threshold: 90%)
- CPU load (default threshold: 95%)
- Disk space (default threshold: 85%)
- Database connectivity
- API server responsiveness
- Persistence layer status

Access health status:
```bash
# CLI
./fungal-cli health

# HTTP
curl http://localhost:8080/api/health
```

### Metrics Export

Prometheus format metrics are available at:
```
http://localhost:9090/metrics
```

Metrics include:
- Request counts and latencies
- Node decision success rates
- Energy consumption
- Signal broadcast/received counts
- Constraint violations
- Processing times

### Logs

Logs are stored in the configured directory:
```bash
tail -f logs/fungal.log
```

Log levels can be set via configuration or CLI:
```bash
./fungal-cli config set logging.level debug
```

## Backup and Recovery

### Automatic Backups

Configure automatic backups:
```yaml
backup:
  enabled: true
  schedule:
    enabled: true
    hour: 2
    minute: 0
    type: incremental
  retention_days: 30
```

### Manual Backup

Create backup on demand:
```bash
./fungal-cli backup create
# Returns backup_id: backup_1696420800

# Verify backup
./fungal-cli backup list
```

### Recovery

Restore from backup:
```bash
./fungal-cli backup restore backup_1696420800
```

Restore to point-in-time:
```bash
# Via API
curl -X POST http://localhost:8080/api/restore/point-in-time \
  -H "Content-Type: application/json" \
  -d '{"timestamp": 1696420800}'
```

## Security Configuration

### User Management

Add users:
```bash
./fungal-cli security add-user alice password123
```

Set permissions:
```bash
# Add admin user
./fungal-cli security add-user admin_user password123
```

### Authentication

Configure authentication method:
```yaml
security:
  auth_method: 1  # token-based
  token_expiry_hours: 24
```

### HTTPS/TLS

Enable HTTPS:
```yaml
security:
  enable_https: true
  certificate_file: /path/to/cert.pem
  private_key_file: /path/to/key.pem
```

## Troubleshooting

### Server Won't Start

Check configuration file:
```bash
cat config.yaml | less
```

Check logs:
```bash
tail -50 logs/fungal.log
```

Verify port is available:
```bash
lsof -i :8080
```

### Health Check Failures

View detailed health status:
```bash
./fungal-cli health
```

Common issues:
- **Memory**: Reduce worker thread count or increase system RAM
- **Disk**: Clean up old logs or backups
- **Database**: Check database connectivity and disk space

### Performance Tuning

Adjust thread count:
```yaml
threading:
  num_threads: 8
```

Adjust metrics retention:
```yaml
metrics:
  retention_hours: 24
```

## Deployment Patterns

### Development

```bash
./fungal-server -c config.dev.yaml
```

### Staging

```bash
./fungal-server -c config.staging.yaml
```

### Production

For high availability:
1. Deploy 3+ nodes for quorum
2. Enable clustering
3. Configure automated backups
4. Set up external database (PostgreSQL/MySQL)
5. Enable HTTPS/TLS
6. Configure monitoring and alerting

Example 3-node production setup:
```bash
# Each node
./fungal-server -c config.prod.yaml

# Load balancer frontend for API requests
# Cluster handles automatic failover
```

## Graceful Shutdown

The server responds to SIGTERM and SIGINT:

```bash
kill -TERM <pid>
# or Ctrl+C
```

Shutdown sequence:
1. Stop accepting new requests
2. Wait for in-flight requests to complete
3. Persist state to backup
4. Flush logs
5. Close database connections
6. Exit cleanly

## Performance Considerations

- **Memory**: ~50MB base + ~1MB per active node
- **CPU**: Scales with node decision frequency
- **Disk**: Determined by log rotation and backup retention
- **Network**: Clustering adds heartbeat traffic (~1KB every 5s per peer)

## Maintenance

### Log Rotation

Configured automatically in `logging` section:
```yaml
logging:
  max_file_size_mb: 10
  max_files: 3
```

### Database Maintenance

For SQLite, cleanup happens automatically. For PostgreSQL/MySQL:
```bash
./fungal-cli database vacuum
```

### Cleanup Old Backups

Automatic based on retention policy:
```yaml
backup:
  retention_days: 30
```

Manual cleanup:
```bash
./fungal-cli backup cleanup
```

## Support and Documentation

- Configuration: See `config.yaml.example`
- API Reference: See `PRODUCTION_API.md`
- Monitoring: See `PRODUCTION_MONITORING.md`
- Security: See `PRODUCTION_SECURITY.md`
