# Fungal Production System Monitoring Guide

## Overview

The Fungal Production System provides comprehensive monitoring capabilities for observability and operational awareness in production environments.

## Built-in Monitoring

### Health Check System

Automatic health checks monitor system vitality every 30 seconds:

**Monitored Components:**
- Memory usage (threshold: 90%)
- CPU load (threshold: 95%)
- Disk space (threshold: 85%)
- Database connectivity
- API server responsiveness
- Persistence layer status

**Health Status:**
- `HEALTHY` - All metrics within acceptable ranges
- `DEGRADED` - Some metrics elevated, service continues
- `UNHEALTHY` - Critical issues, service may be impaired
- `UNKNOWN` - Component status unknown

**Access Health Status:**

```bash
# CLI
./fungal-cli health

# HTTP API
curl http://localhost:8080/api/health
```

**Response Example:**
```json
{
  "timestamp": 1696420800,
  "memory": "healthy",
  "cpu": "healthy",
  "disk": "healthy",
  "database": "healthy",
  "api_server": "healthy",
  "persistence": "healthy",
  "overall": "healthy"
}
```

### Readiness and Liveness Probes

For Kubernetes and orchestration platforms:

**Liveness Probe** (Is the service running?)
```bash
curl http://localhost:8080/health
```

Response code `200` indicates service is alive.

**Readiness Probe** (Can it serve traffic?)
```bash
curl http://localhost:8080/status
```

Response code `200` with `"ready": true` indicates ready.

## Metrics Collection

### Available Metrics

The system automatically collects:

**Request Metrics:**
- `requests_total` - Total requests received (counter)
- `request_latency_ms` - Request processing time (histogram)
- `requests_in_flight` - Currently processing requests (gauge)

**Node Metrics:**
- `node_{id}_decisions` - Total decisions by node
- `node_{id}_successes` - Successful decisions
- `node_{id}_energy` - Current energy level
- `node_{id}_processing_time_ms` - Decision latency

**System Metrics:**
- `signals_broadcast` - Signals sent (counter)
- `signals_received` - Signals received (counter)
- `constraint_violations` - Safety constraint violations

**Resource Metrics:**
- `memory_usage_mb` - Current memory consumption
- `cpu_usage_percent` - CPU utilization
- `disk_usage_percent` - Disk space usage

### Metrics Access

**Via HTTP API:**
```bash
# All metrics
curl http://localhost:8080/api/metrics

# Prometheus format
curl http://localhost:8080/metrics
```

**Via CLI:**
```bash
./fungal-cli metrics
```

**Response Format:**
```json
{
  "metrics": {
    "counters": {
      "requests_total": 15423,
      "signals_broadcast": 8921
    },
    "gauges": {
      "memory_usage_mb": 245.3,
      "cpu_usage_percent": 34.2,
      "disk_usage_percent": 62.1
    },
    "histograms": {
      "request_latency_ms": {
        "count": 15423,
        "min": 1.2,
        "max": 456.7,
        "mean": 45.3
      }
    }
  }
}
```

### Custom Metrics

Record application-specific metrics:

```bash
# Via API
curl -X POST http://localhost:8080/api/metrics/record \
  -H "Content-Type: application/json" \
  -d '{
    "name": "custom_operation_duration_ms",
    "value": 234.5
  }'

# Via CLI (programmatic)
# Use REST API endpoint above
```

## Prometheus Integration

### Setup

1. Configure Prometheus scrape target:

```yaml
# prometheus.yml
scrape_configs:
  - job_name: 'fungal'
    static_configs:
      - targets: ['localhost:8080']
    metrics_path: '/metrics'
    scrape_interval: 30s
```

2. Export metrics endpoint:
```bash
# Prometheus format available at
http://localhost:8080/metrics
```

### Prometheus Queries

**Request Rate (requests/second):**
```promql
rate(requests_total[1m])
```

**P99 Request Latency:**
```promql
histogram_quantile(0.99, rate(request_latency_ms_bucket[5m]))
```

**Memory Usage:**
```promql
memory_usage_mb
```

**Decision Success Rate:**
```promql
node_decisions_total / node_successes_total
```

### Grafana Dashboard

Example Grafana dashboard panels:

**System Health:**
```
overall_health{instance="localhost:8080"}
```

**Request Metrics:**
- Rate: `rate(requests_total[1m])`
- Latency: `request_latency_ms`

**Node Performance:**
```
node_{job_id}_success_rate
```

**Resource Usage:**
- Memory: `memory_usage_mb`
- CPU: `cpu_usage_percent`
- Disk: `disk_usage_percent`

## Logging

### Log Levels

Configure logging level:

```yaml
logging:
  level: info  # trace, debug, info, warn, error, critical
```

**Log Levels:**
- `trace` - Very detailed information
- `debug` - Debugging information
- `info` - General information messages
- `warn` - Warning messages
- `error` - Error messages
- `critical` - Critical error messages

### Log Output

**Console Output:**
Enabled by default for development/testing.

**File Output:**
Logs stored in configured directory (default: `logs/`)

Configuration:
```yaml
logging:
  directory: logs
  file_name: fungal.log
  max_file_size_mb: 10
  max_files: 3
  console_output: true
```

### Log Rotation

Automatic log rotation configured:
- File size limit: 10MB (configurable)
- Max files: 3 (configurable)
- Older logs automatically deleted

### Structured Logging

Logs include structured information:

```json
{
  "timestamp": "2023-10-03T12:34:56.789Z",
  "level": "info",
  "message": "Node verifier_1 decision: verify_claim [SUCCESS]",
  "node_id": "verifier_1",
  "decision": "verify_claim",
  "result": "success"
}
```

### Accessing Logs

**View logs:**
```bash
tail -f logs/fungal.log

# Search logs
grep "error" logs/fungal.log

# JSON parsing
cat logs/fungal.log | jq 'select(.level == "error")'
```

**Via API:**
```bash
curl http://localhost:8080/api/logs?level=error&limit=100
```

## Alerting

### Health-based Alerts

Configure alerts based on health status:

```bash
# Monitor health status
watch -n30 './fungal-cli health'

# Alert if degraded
watch -n30 'if [ "$(curl -s http://localhost:8080/api/health | jq -r .overall)" != "healthy" ]; then echo "ALERT: Health degraded"; fi'
```

### Prometheus Alerting

Example alert rules (alert.rules.yml):

```yaml
groups:
  - name: fungal_system
    rules:
      - alert: HighMemoryUsage
        expr: memory_usage_mb > 500
        for: 5m
        annotations:
          summary: "High memory usage detected"

      - alert: HighErrorRate
        expr: rate(errors_total[5m]) > 0.05
        for: 5m
        annotations:
          summary: "High error rate detected"

      - alert: SlowRequests
        expr: histogram_quantile(0.99, rate(request_latency_ms_bucket[5m])) > 1000
        for: 5m
        annotations:
          summary: "P99 latency above 1 second"
```

### Custom Alerts

Implement custom alert logic:

```cpp
// In your monitoring code
if (HealthCheck::instance().get_full_health_status()["overall"] != "healthy") {
    // Trigger alert
    Logger::instance().critical("System health degraded!");
    // Send to alert service
}
```

## Audit Logging

### Audit Trail

All significant operations are logged:

**Recorded Events:**
- Authentication attempts (success/failure)
- User permission changes
- Configuration modifications
- Backup operations
- Cluster membership changes
- Constraint violations
- Critical errors

**Access Audit Trail:**

```bash
# Via CLI
./fungal-cli logs  # View audit trail

# Via API
curl http://localhost:8080/api/security/audit
```

**Audit Log Format:**
```json
{
  "timestamp": 1696420800,
  "username": "admin",
  "action": "backup_created",
  "resource": "backup_1696420800",
  "result": "success",
  "ip_address": "192.168.1.10"
}
```

## Performance Monitoring

### Throughput Metrics

**Request Throughput:**
```bash
# Requests per second
curl http://localhost:8080/api/metrics | jq '.metrics.counters.requests_total'
```

**Decision Throughput:**
```bash
# Decisions per node
curl http://localhost:8080/api/metrics | jq '.metrics.counters[] | select(.key | contains("decisions"))'
```

### Latency Metrics

**Request Latency:**
```bash
curl http://localhost:8080/api/metrics | \
  jq '.metrics.histograms.request_latency_ms'
```

Response includes: count, min, max, mean, variance

**Node Processing Time:**
```bash
curl http://localhost:8080/api/metrics | \
  jq '.metrics.histograms[] | select(.key | contains("processing_time"))'
```

### Resource Utilization

**Memory Usage:**
```bash
./fungal-cli health | jq '.metrics.memory_mb'
```

**CPU Usage:**
```bash
./fungal-cli health | jq '.metrics.cpu_percent'
```

**Disk Usage:**
```bash
./fungal-cli health | jq '.metrics.disk_percent'
```

## Distributed Tracing

### Request Tracing

Each request receives a unique ID:

```bash
curl http://localhost:8080/api/system/overview \
  -H "X-Request-ID: trace_12345"
```

Request IDs appear in:
- Logs
- Metrics
- API responses
- Audit trail

### Correlation IDs

For tracking related operations across services:

```bash
# API request
curl -X POST http://localhost:8080/api/nodes/verifier_1/decide \
  -H "X-Correlation-ID: cor_67890"

# Logs will include correlation ID
# Allows tracing related decisions
```

## Dashboard Setup

### CLI Dashboard

Monitor with CLI:
```bash
# Watch system status
watch -n5 './fungal-cli status'

# Watch health
watch -n5 './fungal-cli health'

# Watch metrics
watch -n5 './fungal-cli metrics'
```

### Web Dashboard

Web dashboard backend available:

```bash
./fungal-dashboard --port 9000
```

Access at: `http://localhost:9000/dashboard`

**Dashboard Features:**
- System overview
- Real-time metrics
- Health status
- Backup management
- Cluster visualization
- Audit trail viewing

## Alerting Integration

### Email Alerts

Configure email notifications for critical issues:

```yaml
alerting:
  enabled: true
  channels:
    - type: email
      address: ops@example.com
      triggers:
        - health_degraded
        - high_error_rate
        - backup_failed
```

### Webhook Alerts

Send alerts to external systems:

```yaml
alerting:
  enabled: true
  channels:
    - type: webhook
      url: https://alerts.example.com/webhook
      triggers:
        - constraint_violation
        - authentication_failure
```

### PagerDuty Integration

Critical alerts automatically create incidents:

```yaml
alerting:
  enabled: true
  channels:
    - type: pagerduty
      api_key: ${PAGERDUTY_API_KEY}
      triggers:
        - system_unhealthy
        - cluster_partitioned
```

## Troubleshooting

### High Memory Usage

Check current usage:
```bash
./fungal-cli health | grep memory
```

Potential solutions:
1. Reduce log retention period
2. Reduce metrics retention period
3. Increase thread pool tasks timeout
4. Check for memory leaks in logs

### High CPU Usage

```bash
./fungal-cli metrics | jq '.metrics.gauges.cpu_usage_percent'
```

Potential solutions:
1. Reduce decision frequency
2. Increase thread pool size
3. Check for infinite loops in decision logic

### Database Performance

```bash
./fungal-cli health | grep database
```

Optimize with:
1. Increase connection pool size
2. Add database indexes
3. Archive old audit logs

## Best Practices

1. **Regular Health Checks**: Monitor health status at least every 5 minutes
2. **Metric Retention**: Keep metrics for at least 24 hours for trend analysis
3. **Log Rotation**: Implement log rotation to prevent disk space issues
4. **Backup Verification**: Verify backups weekly to catch corruption early
5. **Alerting Tuning**: Start with conservative thresholds, refine over time
6. **Audit Review**: Review audit logs weekly for security issues
7. **Capacity Planning**: Monitor trends to predict resource needs
8. **Incident Response**: Document and review production incidents
