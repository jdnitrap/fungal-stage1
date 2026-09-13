# Fungal Production System REST API Reference

## Base URL

```
http://localhost:8080
https://localhost:8443  # if HTTPS enabled
```

## Authentication

Authentication method is configured in `security.auth_method`:

- `0`: No authentication (development only)
- `1`: Token-based (recommended)
- `2`: Basic HTTP authentication
- `3`: Certificate-based
- `4`: OAuth2

### Token Authentication

1. Generate token:
```bash
curl -X POST http://localhost:8080/auth/token \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "secret"}'
```

Response:
```json
{
  "token": "admin_1696420800",
  "expires_in": 86400
}
```

2. Use token in requests:
```bash
curl http://localhost:8080/api/system/overview \
  -H "Authorization: Bearer admin_1696420800"
```

## Health & Status Endpoints

### GET /health

System health check (no auth required).

**Response:**
```json
{
  "status": "healthy",
  "timestamp": 1696420800
}
```

**Status values:** `healthy`, `degraded`, `unhealthy`

### GET /status

Detailed system status.

**Response:**
```json
{
  "running": true,
  "uptime_seconds": 3600,
  "version": "1.0.0"
}
```

### GET /metrics

System metrics snapshot.

**Response:**
```json
{
  "requests": 1523,
  "errors": 2,
  "avg_latency_ms": 45.3,
  "p99_latency_ms": 234.5
}
```

## System Endpoints

### GET /api/system/overview

Complete system overview including health, metrics, and cluster status.

**Response:**
```json
{
  "system": {
    "initialized": true,
    "running": true,
    "timestamp": 1696420800
  },
  "health": {
    "overall": "healthy",
    "memory": "healthy",
    "cpu": "healthy",
    "disk": "healthy",
    "database": "healthy",
    "api_server": "healthy",
    "persistence": "healthy"
  },
  "metrics": {
    "counters": {},
    "gauges": {},
    "histograms": {}
  }
}
```

### GET /api/system/config

System configuration (safe values only, passwords excluded).

**Response:**
```json
{
  "config": {
    "api_port": 8080,
    "database_type": "sqlite",
    "clustering_enabled": false,
    "backup_enabled": true
  }
}
```

## Health Monitoring Endpoints

### GET /api/health

Detailed health check results for all components.

**Response:**
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

### GET /api/health/{component}

Check specific component health (e.g., `database`, `api_server`).

**Response:**
```json
{
  "component": "database",
  "status": "healthy",
  "message": "Database connection OK",
  "check_time": 1696420800
}
```

## Metrics Endpoints

### GET /api/metrics

All system metrics.

**Response:**
```json
{
  "metrics": {
    "counters": {
      "requests_total": 1523,
      "errors_total": 2
    },
    "gauges": {
      "memory_usage_mb": 125.4,
      "cpu_usage_percent": 34.2
    },
    "histograms": {
      "request_latency_ms": {
        "count": 1523,
        "min": 2.1,
        "max": 234.5,
        "mean": 45.3
      }
    }
  }
}
```

### GET /api/metrics/{metric_name}

Specific metric data.

**Response:**
```json
{
  "name": "request_latency_ms",
  "data": [45.3, 52.1, 48.9, ...]
}
```

### POST /api/metrics/record

Record custom metric value.

**Request:**
```json
{
  "name": "custom_metric",
  "value": 42.5
}
```

**Response:**
```json
{
  "status": "recorded",
  "metric": "custom_metric"
}
```

### GET /metrics (Prometheus Format)

Export metrics in Prometheus format for scraping.

**Response (text/plain):**
```
# TYPE requests_total counter
requests_total 1523
# TYPE memory_usage_mb gauge
memory_usage_mb 125.4
```

## Backup Endpoints

### GET /api/backups

List all backups.

**Response:**
```json
{
  "backups": [
    {
      "backup_id": "backup_1696420800",
      "type": 0,
      "status": 2,
      "timestamp": 1696420800,
      "location": "backups/backup_1696420800",
      "size_bytes": 1024000,
      "verified": true
    }
  ]
}
```

### GET /api/backups/{backup_id}

Get backup information.

**Response:**
```json
{
  "backup_id": "backup_1696420800",
  "status": "verified",
  "timestamp": 1696420800,
  "size_bytes": 1024000,
  "checksum": "abc123def456"
}
```

### POST /api/backups/create

Create new backup.

**Request:**
```json
{
  "type": "incremental"
}
```

**Response:**
```json
{
  "status": "started",
  "backup_id": "backup_1696420801"
}
```

### POST /api/backups/restore

Restore from backup.

**Request:**
```json
{
  "backup_id": "backup_1696420800"
}
```

**Response:**
```json
{
  "status": "restored",
  "backup_id": "backup_1696420800"
}
```

### POST /api/backups/verify

Verify backup integrity.

**Request:**
```json
{
  "backup_id": "backup_1696420800"
}
```

**Response:**
```json
{
  "status": "verified",
  "backup_id": "backup_1696420800",
  "verified": true
}
```

### DELETE /api/backups/{backup_id}

Delete backup.

**Response:**
```json
{
  "status": "deleted",
  "backup_id": "backup_1696420800"
}
```

## Cluster Endpoints

### GET /api/cluster

Get cluster status.

**Response:**
```json
{
  "cluster": {
    "cluster_id": "production-cluster",
    "node_id": "node1",
    "is_leader": true,
    "leader_id": "node1",
    "cluster_size": 3,
    "is_partitioned": false,
    "nodes": [
      {
        "id": "node1",
        "hostname": "server1.example.com",
        "port": 9001,
        "state": 1,
        "last_heartbeat": "1696420800"
      }
    ]
  }
}
```

**Node states:** 0=unknown, 1=healthy, 2=degraded, 3=unhealthy, 4=disconnected

### GET /api/cluster/nodes

List all cluster nodes.

**Response:**
```json
{
  "nodes": [
    {
      "id": "node1",
      "hostname": "server1.example.com",
      "state": "healthy"
    },
    {
      "id": "node2",
      "hostname": "server2.example.com",
      "state": "healthy"
    }
  ]
}
```

### GET /api/cluster/leader

Get current leader information.

**Response:**
```json
{
  "leader_id": "node1",
  "hostname": "server1.example.com",
  "is_current_node": true
}
```

## Security Endpoints

### GET /api/security

Security configuration.

**Response:**
```json
{
  "security": {
    "auth_method": "token",
    "certificate_loaded": true,
    "private_key_loaded": true,
    "total_users": 5,
    "active_tokens": 3
  }
}
```

### POST /api/security/users

Create new user.

**Request:**
```json
{
  "username": "alice",
  "password": "secure_password",
  "is_admin": false
}
```

**Response:**
```json
{
  "status": "created",
  "username": "alice"
}
```

### GET /api/security/users

List users (admin only).

**Response:**
```json
{
  "users": [
    {
      "username": "admin",
      "is_admin": true,
      "is_active": true
    }
  ]
}
```

### DELETE /api/security/users/{username}

Delete user (admin only).

**Response:**
```json
{
  "status": "deleted",
  "username": "alice"
}
```

### GET /api/security/audit

Get audit log (admin only).

**Response:**
```json
{
  "entries": [
    {
      "timestamp": 1696420800,
      "username": "admin",
      "action": "authenticate",
      "success": true
    }
  ],
  "total_entries": 42
}
```

## Node Decision Endpoints

### POST /api/nodes

Create autonomous node.

**Request:**
```json
{
  "node_id": "verifier_1",
  "node_type": "verifier"
}
```

**Response:**
```json
{
  "status": "created",
  "node_id": "verifier_1"
}
```

### GET /api/nodes/{node_id}

Get node information.

**Response:**
```json
{
  "node_id": "verifier_1",
  "state": "active",
  "energy": 95.2,
  "success_rate": 0.94,
  "decisions": 256
}
```

### POST /api/nodes/{node_id}/decide

Send decision request to node.

**Request:**
```json
{
  "goal": "verify_claim",
  "parameters": {
    "claim": "2+2=4",
    "context": "mathematical"
  }
}
```

**Response:**
```json
{
  "decision_id": "dec_1696420800",
  "node_id": "verifier_1",
  "result": true,
  "confidence": 0.99
}
```

## Error Responses

All error responses follow this format:

```json
{
  "error": "Error message",
  "error_code": "INVALID_REQUEST",
  "timestamp": 1696420800
}
```

**Common error codes:**
- `INVALID_REQUEST`: Malformed request
- `UNAUTHORIZED`: Authentication required/failed
- `FORBIDDEN`: Permission denied
- `NOT_FOUND`: Resource not found
- `INTERNAL_ERROR`: Server error
- `SERVICE_UNAVAILABLE`: Service temporarily down

**HTTP Status Codes:**
- `200 OK`: Success
- `400 Bad Request`: Invalid input
- `401 Unauthorized`: Auth required
- `403 Forbidden`: Insufficient permissions
- `404 Not Found`: Resource not found
- `500 Internal Server Error`: Server error
- `503 Service Unavailable`: Service down

## Rate Limiting

API rate limits (when enabled):
- Default: 1000 requests per minute per IP
- Header: `X-RateLimit-Remaining`

## Pagination

List endpoints support pagination:

```bash
GET /api/backups?limit=10&offset=0
```

**Parameters:**
- `limit`: Results per page (default: 100, max: 1000)
- `offset`: Starting position (default: 0)

**Response includes:**
```json
{
  "data": [...],
  "pagination": {
    "limit": 10,
    "offset": 0,
    "total": 42
  }
}
```

## Examples

### Get System Health

```bash
curl -X GET http://localhost:8080/api/health
```

### Create and Restore Backup

```bash
# Create backup
BACKUP_ID=$(curl -s -X POST http://localhost:8080/api/backups/create \
  -H "Authorization: Bearer TOKEN" | jq -r '.backup_id')

# Restore backup
curl -X POST http://localhost:8080/api/backups/restore \
  -H "Authorization: Bearer TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"backup_id\": \"$BACKUP_ID\"}"
```

### Monitor Metrics

```bash
# Every 10 seconds
watch -n10 'curl -s http://localhost:8080/api/metrics | jq ".metrics.gauges"'
```

### Check Cluster Status

```bash
curl http://localhost:8080/api/cluster | jq '.cluster'
```
