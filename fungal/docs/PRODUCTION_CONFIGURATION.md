# Fungal Production System Configuration Guide

## Configuration Overview

The Fungal Production System is configured via YAML files with support for environment variable overrides. All configuration options are documented here with examples for common deployment scenarios.

## Configuration File Format

```yaml
# Comments start with #
section:
  subsection:
    key: value
    list_key:
      - item1
      - item2
```

## Environment Variables

Override any configuration value with environment variables:

```bash
# Convention: FUNGAL_<SECTION>_<KEY>=value
export FUNGAL_API_PORT=9000
export FUNGAL_DATABASE_PASSWORD="secret_password"
export FUNGAL_LOGGING_LEVEL=debug

./fungal-server -c config.yaml
```

## Complete Configuration Reference

### API Server Configuration

```yaml
api:
  # Port to listen on
  port: 8080
  
  # Bind address
  host: 0.0.0.0
  
  # Enable CORS for cross-origin requests
  enable_cors: true
  
  # CORS allowed origins (if CORS enabled)
  cors_origins:
    - http://localhost:3000
    - https://app.example.com
  
  # Request timeout in seconds
  request_timeout_seconds: 30
  
  # Max request body size (MB)
  max_request_size_mb: 10
  
  # Thread pool size for handling requests
  thread_pool_size: 16
  
  # Enable request logging
  log_requests: true
```

**Environment Overrides:**
```bash
export FUNGAL_API_PORT=9000
export FUNGAL_API_HOST=127.0.0.1
```

### Logging Configuration

```yaml
logging:
  # Log level: trace, debug, info, warn, error, critical
  level: info
  
  # Log directory
  directory: logs
  
  # Log file name
  file_name: fungal.log
  
  # Maximum log file size (MB)
  max_file_size_mb: 10
  
  # Maximum number of rotated log files to keep
  max_files: 5
  
  # Log to console
  console_output: true
  
  # Log format
  format: json  # json or text
  
  # Include timestamps in logs
  timestamps: true
  
  # Include thread IDs
  include_thread_id: true
```

**Environment Overrides:**
```bash
export FUNGAL_LOGGING_LEVEL=debug
export FUNGAL_LOGGING_DIRECTORY=/var/log/fungal
```

### Database Configuration

#### SQLite (Development/Small Deployments)

```yaml
database:
  type: sqlite
  file: fungal.db
  
  # Connection timeout (seconds)
  timeout: 5
  
  # Enable WAL mode (better concurrency)
  wal_mode: true
  
  # Synchronous mode: OFF, NORMAL, FULL
  synchronous: NORMAL
  
  # Cache size (pages)
  cache_size: 2000
```

#### PostgreSQL (Recommended for Production)

```yaml
database:
  type: postgresql
  
  # Connection details
  host: localhost
  port: 5432
  username: fungal
  password: ${DB_PASSWORD}  # From environment
  database: fungal_prod
  
  # Connection pool
  pool_size: 20
  max_overflow: 10
  pool_timeout: 30
  
  # SSL mode: disable, allow, prefer, require
  ssl_mode: require
  
  # Connection retries
  retry_attempts: 3
  retry_delay_seconds: 5
```

#### MySQL/MariaDB

```yaml
database:
  type: mysql
  
  # Connection details
  host: localhost
  port: 3306
  username: fungal
  password: ${DB_PASSWORD}
  database: fungal_prod
  
  # Connection pool
  pool_size: 20
  max_overflow: 10
  
  # SSL/TLS
  ssl_enabled: true
  ssl_ca: /path/to/ca.pem
```

**Database Initialization:**
```bash
# First run creates schema automatically
./fungal-server -c config.yaml

# Or manually initialize
./fungal-cli database init
```

### Threading Configuration

```yaml
threading:
  # Number of worker threads (0 = auto-detect)
  num_threads: 0
  
  # Task queue size
  task_queue_size: 1000
  
  # Thread naming format
  thread_name_format: "fungal-worker-{id}"
  
  # Thread stack size (KB)
  stack_size: 8192
  
  # Allow dynamic thread resizing
  dynamic_resize: false
```

### Security Configuration

```yaml
security:
  # Authentication method
  # 0 = none, 1 = token, 2 = basic, 3 = certificate, 4 = oauth2
  auth_method: 1
  
  # Password policy
  password_min_length: 12
  password_require_uppercase: true
  password_require_lowercase: true
  password_require_digits: true
  password_require_special_chars: true
  
  # Token settings
  token_expiry_hours: 24
  token_refresh_enabled: true
  
  # HTTPS/TLS
  enable_https: false
  certificate_file: /etc/ssl/certs/server.pem
  private_key_file: /etc/ssl/private/key.pem
  tls_version: "1.3"  # Minimum TLS version
  
  # Encryption
  encryption_algorithm: AES_256_GCM
  
  # Rate limiting
  rate_limit_enabled: true
  rate_limit_requests_per_minute: 1000
  rate_limit_per_ip: true
  
  # Session management
  session_timeout_minutes: 60
  session_idle_timeout_minutes: 30
```

### Clustering Configuration

```yaml
clustering:
  # Enable clustering
  enabled: false
  
  # Cluster identification
  cluster_id: production
  node_id: node1
  
  # Cluster discovery
  discovery_type: static  # static or consul
  
  # Static peer addresses (if discovery_type: static)
  peer_addresses:
    - node2.example.com:9001
    - node3.example.com:9001
  
  # Consensus algorithm settings
  heartbeat_interval_ms: 5000
  election_timeout_ms: 15000
  snapshot_interval: 10000
  
  # Replication
  replication_batch_size: 100
  replication_timeout_seconds: 10
```

**Consul Integration (Dynamic Discovery):**
```yaml
clustering:
  enabled: true
  discovery_type: consul
  consul:
    host: localhost
    port: 8500
    service_name: fungal
    datacenter: dc1
```

### Persistence Configuration

```yaml
persistence:
  # State storage directory
  state_directory: state/
  
  # Snapshot directory
  snapshot_directory: snapshots/
  
  # Enable automatic snapshots
  enable_auto_snapshot: true
  
  # Auto-snapshot interval (minutes)
  snapshot_interval_minutes: 60
  
  # Compression for snapshots
  compress_snapshots: true
  compression_level: 6  # 1-9, higher = smaller but slower
```

### Backup Configuration

```yaml
backup:
  # Enable backup system
  enabled: true
  
  # Backup directory
  directory: backups/
  
  # Retention policy
  retention_days: 30
  max_backups: 100
  
  # Encryption
  encrypt_backups: true
  encryption_key: ${BACKUP_KEY}  # From environment
  
  # Compression
  compress_backups: true
  
  # Backup scheduling
  schedule:
    enabled: true
    
    # Full backup daily at 2 AM
    full:
      hour: 2
      minute: 0
      day_of_week: "*"  # Every day
    
    # Incremental backup hourly
    incremental:
      enabled: true
      interval_hours: 6
  
  # Parallel backup threads
  parallel_threads: 2
  
  # Backup verification
  verify_after_backup: true
  
  # Upload to remote storage
  remote_backup:
    enabled: false
    provider: s3  # s3, gcs, azure
    bucket: fungal-backups
    region: us-east-1
```

### Health Check Configuration

```yaml
health:
  # Check interval (seconds)
  check_interval_seconds: 30
  
  # Thresholds
  memory_threshold_percent: 90
  cpu_threshold_percent: 95
  disk_threshold_percent: 85
  
  # Component-specific checks
  database_check_enabled: true
  api_check_enabled: true
  persistence_check_enabled: true
  
  # Liveness probe timeout (seconds)
  liveness_timeout: 5
  
  # Readiness probe timeout (seconds)
  readiness_timeout: 10
```

### Metrics Configuration

```yaml
metrics:
  # Enable metrics collection
  enabled: true
  
  # Export interval (seconds)
  export_interval_seconds: 60
  
  # Retention period (hours)
  retention_hours: 24
  
  # Prometheus metrics
  prometheus_enabled: true
  prometheus_port: 9090
  prometheus_path: /metrics
  
  # Histogram buckets (milliseconds)
  histogram_buckets:
    - 1
    - 5
    - 10
    - 50
    - 100
    - 500
    - 1000
    - 5000
  
  # Enable high-precision metrics (uses more memory)
  high_precision: false
```

### Node Configuration

```yaml
node:
  # Node name
  name: fungal-default
  
  # Node description
  description: Default Fungal Production Node
  
  # Node version
  version: 1.0.0
  
  # Node location/tags
  location: datacenter-1
  tags:
    - production
    - primary
```

### System Tuning

```yaml
tuning:
  # Enable performance monitoring
  enable_performance_monitoring: true
  
  # Enable debug logging
  enable_debug_logging: false
  
  # Enable CPU profiling
  enable_profiling: false
  
  # Profile output directory
  profile_directory: profiles/
  
  # Enable memory limit enforcement
  enforce_memory_limit: false
  memory_limit_mb: 1024
  
  # Nice level for process (-20 to 20)
  nice_level: 0
  
  # CPU affinity (bind to specific cores)
  cpu_affinity: []  # Empty = all cores available
```

## Configuration Examples

### Development Configuration

```yaml
api:
  port: 8080
  host: 127.0.0.1
  enable_cors: true

logging:
  level: debug
  console_output: true

database:
  type: sqlite
  file: dev.db

threading:
  num_threads: 2

security:
  auth_method: 0  # No auth for development

clustering:
  enabled: false

backup:
  enabled: false
```

### Staging Configuration

```yaml
api:
  port: 8080
  host: 0.0.0.0
  enable_cors: true

logging:
  level: info
  directory: /var/log/fungal

database:
  type: postgresql
  host: staging-db.internal
  port: 5432
  username: fungal
  password: ${DB_PASSWORD}
  database: fungal_staging

threading:
  num_threads: 4

security:
  auth_method: 1
  enable_https: false

clustering:
  enabled: false

backup:
  enabled: true
  directory: /backups/fungal
  retention_days: 7
```

### Production Configuration

```yaml
api:
  port: 8080
  host: 0.0.0.0
  enable_cors: true
  cors_origins:
    - https://app.example.com

logging:
  level: info
  directory: /var/log/fungal
  max_file_size_mb: 100
  max_files: 10

database:
  type: postgresql
  host: prod-db.internal
  port: 5432
  username: fungal
  password: ${DB_PASSWORD}
  database: fungal_prod
  pool_size: 50
  ssl_mode: require

threading:
  num_threads: 0  # Auto-detect
  thread_pool_size: 32

security:
  auth_method: 1
  enable_https: true
  certificate_file: /etc/ssl/certs/fungal.pem
  private_key_file: /etc/ssl/private/fungal.key
  password_min_length: 12
  rate_limit_enabled: true

clustering:
  enabled: true
  cluster_id: production
  peer_addresses:
    - node2.prod.internal:9001
    - node3.prod.internal:9001

backup:
  enabled: true
  directory: /backups/fungal
  retention_days: 90
  schedule:
    enabled: true
    full:
      hour: 2
      minute: 0
  remote_backup:
    enabled: true
    provider: s3
    bucket: fungal-backups-prod
    region: us-east-1
```

### High-Availability Configuration

```yaml
api:
  port: 8080
  host: 0.0.0.0
  thread_pool_size: 64

logging:
  level: warn  # Reduce I/O
  directory: /var/log/fungal

database:
  type: postgresql
  host: db-cluster.internal
  port: 5432
  pool_size: 100
  max_overflow: 20

threading:
  num_threads: 0
  thread_pool_size: 64

security:
  auth_method: 1
  enable_https: true

clustering:
  enabled: true
  heartbeat_interval_ms: 3000
  election_timeout_ms: 10000

backup:
  enabled: true
  parallel_threads: 4
  remote_backup:
    enabled: true
    provider: s3
```

## Validation and Testing

### Validate Configuration

```bash
# Syntax check
./fungal-cli config validate

# Test configuration load
./fungal-server -c config.yaml --validate-only

# Show resolved configuration (with environment overrides)
./fungal-cli config show
```

### Test Configuration Changes

```bash
# Load config in staging first
./fungal-server -c config.yaml --environment=staging

# Run integration tests
make test-integration

# Verify no regressions
make test-smoke
```

## Configuration Migration

### Upgrading Configuration Format

When upgrading between versions:

```bash
# Backup current configuration
cp config.yaml config.yaml.backup

# Auto-migrate configuration
./fungal-cli config migrate config.yaml --to-version 2.0

# Review changes
diff config.yaml.backup config.yaml

# Deploy
./fungal-server -c config.yaml
```

## Troubleshooting Configuration

### Configuration Not Applied

1. **Check syntax:**
   ```bash
   yamllint config.yaml
   ```

2. **Verify file permissions:**
   ```bash
   ls -la config.yaml
   ```

3. **Test configuration load:**
   ```bash
   ./fungal-cli config validate config.yaml
   ```

### Environment Variables Not Overriding

1. **Verify variable name format:**
   ```bash
   # Correct: FUNGAL_SECTION_KEY
   export FUNGAL_API_PORT=9000
   
   # Incorrect (won't work):
   export API_PORT=9000
   ```

2. **Check variable is exported:**
   ```bash
   env | grep FUNGAL
   ```

### Performance Issues

Review these configurations:
- Increase `threading.num_threads`
- Increase `database.pool_size`
- Reduce `logging.level` (from debug to info)
- Enable `tuning.enable_performance_monitoring` to identify bottlenecks
