# Fungal Production System Security Guide

## Overview

Security is a core component of the Fungal Production System. This guide covers authentication, authorization, encryption, audit logging, and security best practices.

## Authentication

### Authentication Methods

Configure authentication in `config.yaml`:

```yaml
security:
  auth_method: 1  # 0=none, 1=token, 2=basic, 3=certificate, 4=oauth2
```

### 1. No Authentication (Development Only)

```yaml
security:
  auth_method: 0
```

**Use case:** Local development only. Never use in production.

### 2. Token-Based Authentication (Recommended)

```yaml
security:
  auth_method: 1
  token_expiry_hours: 24
```

**Process:**
1. User authenticates with username/password
2. Server generates bearer token
3. Token included in `Authorization: Bearer TOKEN` header

**Generate Token:**
```bash
curl -X POST http://localhost:8080/auth/token \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "secret"}'
```

Response:
```json
{
  "token": "admin_1696420800",
  "expires_in": 86400,
  "token_type": "Bearer"
}
```

**Use Token:**
```bash
curl http://localhost:8080/api/system/overview \
  -H "Authorization: Bearer admin_1696420800"
```

**Token Expiration:**
- Tokens expire after configured hours (default: 24)
- Request new token when expired
- Expired tokens receive `401 Unauthorized` response

### 3. Basic HTTP Authentication

```yaml
security:
  auth_method: 2
```

**Usage:**
```bash
curl -u username:password http://localhost:8080/api/system/overview
```

**Security Note:** Only use over HTTPS. Password transmitted in Base64 encoding (easily decoded).

### 4. Certificate-Based Authentication

```yaml
security:
  auth_method: 3
  certificate_file: /etc/ssl/certs/client-cert.pem
  private_key_file: /etc/ssl/private/client-key.pem
```

**Client Certificate Request:**
```bash
curl --cert /path/to/client-cert.pem \
     --key /path/to/client-key.pem \
     https://localhost:8443/api/system/overview
```

### 5. OAuth2 Integration

```yaml
security:
  auth_method: 4
  oauth2:
    provider: "https://oauth.example.com"
    client_id: "${OAUTH2_CLIENT_ID}"
    client_secret: "${OAUTH2_CLIENT_SECRET}"
```

Redirects to external OAuth2 provider for authentication.

## Authorization

### Role-Based Access Control (RBAC)

**User Roles:**
- `admin` - Full system access
- `operator` - Operational commands (start/stop/backup)
- `viewer` - Read-only access
- `auditor` - Audit log access

### User Management

**Add User:**
```bash
./fungal-cli security add-user alice password123
```

**Set Permissions:**
```bash
./fungal-cli security set-permissions alice --role operator
```

**List Users:**
```bash
./fungal-cli security list-users
```

**Remove User:**
```bash
./fungal-cli security remove-user alice
```

### Default Permissions

**Admin Role:**
- Create/delete users
- Modify configuration
- Create/restore backups
- View audit logs
- Manage cluster

**Operator Role:**
- Start/stop nodes
- Create backups
- View metrics
- Manage backups

**Viewer Role:**
- View system status
- View metrics
- View health checks

**Auditor Role:**
- View audit logs
- View access history

### API-Level Authorization

All API endpoints enforce authorization:

```bash
# Success - admin user
curl -H "Authorization: Bearer admin_token" \
  http://localhost:8080/api/security/users

# Failure - viewer user
curl -H "Authorization: Bearer viewer_token" \
  http://localhost:8080/api/security/users
# Response: 403 Forbidden
```

## Encryption

### Data at Rest

**SQLite (Local):**
```yaml
database:
  type: sqlite
  file: fungal.db
```

No native encryption. Protect file permissions:
```bash
chmod 600 fungal.db
chmod 700 data/
```

**PostgreSQL (Recommended):**
Enable encryption at database level:
```sql
-- Enable pgcrypto extension
CREATE EXTENSION pgcrypto;

-- Encrypt sensitive data
ALTER TABLE state ENCRYPT COLUMN sensitive_data;
```

**Full-Disk Encryption:**
Use OS-level encryption (LUKS on Linux, FileVault on macOS):
```bash
# Linux
sudo cryptsetup luksFormat /dev/sdX
sudo cryptsetup luksOpen /dev/sdX data
sudo mkfs.ext4 /dev/mapper/data
```

### Data in Transit

**HTTPS/TLS (Recommended):**
```yaml
security:
  enable_https: true
  certificate_file: /etc/ssl/certs/server-cert.pem
  private_key_file: /etc/ssl/private/server-key.pem
  tls_version: "1.3"  # Use TLS 1.3
```

**Generate Self-Signed Certificate (Development):**
```bash
openssl req -x509 -newkey rsa:4096 -nodes \
  -keyout server-key.pem -out server-cert.pem \
  -days 365
```

**Generate Proper Certificate (Production):**
Use Let's Encrypt:
```bash
sudo certbot certonly --standalone -d fungal.example.com
sudo cp /etc/letsencrypt/live/fungal.example.com/fullchain.pem server-cert.pem
sudo cp /etc/letsencrypt/live/fungal.example.com/privkey.pem server-key.pem
```

### Encryption Algorithms

Configured encryption:
```yaml
security:
  encryption_algorithm: AES_256_GCM  # Recommended
```

**Supported Algorithms:**
- `NONE` - No encryption
- `AES_256_CBC` - AES-256 CBC mode
- `AES_256_GCM` - AES-256 GCM mode (Recommended)

### Database Encryption

Store passwords using strong hashing:
```cpp
// Internally, passwords are hashed with salt
std::string hashed = Security::instance().hash_password("user_password");
bool verified = Security::instance().verify_password("user_password", hashed);
```

## Audit Logging

### Audit Trail

All significant operations logged:

**Logged Events:**
- User authentication (success/failure)
- Authorization decisions (allowed/denied)
- Configuration changes
- User management operations
- Backup creation/restoration
- Cluster changes
- Constraint violations
- API access (optional, for high-security deployments)

**View Audit Log:**
```bash
# CLI
./fungal-cli logs audit

# API
curl http://localhost:8080/api/security/audit \
  -H "Authorization: Bearer admin_token"
```

**Audit Log Format:**
```json
{
  "timestamp": 1696420800,
  "username": "admin",
  "action": "create_backup",
  "resource": "backup_1696420800",
  "result": "success",
  "ip_address": "192.168.1.10",
  "user_agent": "fungal-cli/1.0.0"
}
```

### Log Retention

Configure audit log retention:
```yaml
audit:
  retention_days: 365  # Keep 1 year
  archive_old_logs: true  # Archive after 90 days
```

### Audit Log Security

Protect audit logs:
1. Store on separate, secure storage
2. Restrict read access (admin only)
3. Enable write-once protection (if supported)
4. Monitor for tampering
5. Archive regularly to immutable storage

```bash
# Restrict audit log permissions
chmod 600 logs/audit.log
```

## Secure Configuration

### Secrets Management

**Never commit secrets:**
```bash
# Bad: Secrets in config file
password: "secret123"

# Good: Environment variables
password: "${DB_PASSWORD}"  # Set via: export DB_PASSWORD="..."
```

**Environment Variable Override:**
```bash
# config.yaml references ${VAR_NAME}
# Override at runtime:
export FUNGAL_DATABASE_PASSWORD="secure_password"
export FUNGAL_API_KEY="secret_api_key"

./fungal-server -c config.yaml
```

**Use .env file (Development Only):**
```bash
# .env
DB_PASSWORD=secure_password
API_KEY=secret_key

# Load before running:
set -a
source .env
set +a
./fungal-server -c config.yaml
```

**Production Secret Management:**
Use HashiCorp Vault or AWS Secrets Manager:

```yaml
security:
  vault:
    enabled: true
    server: https://vault.example.com
    token: ${VAULT_TOKEN}
    secrets:
      - path: secret/database/password
        key: password
```

### Configuration Best Practices

1. **Use Strong Defaults:**
   ```yaml
   security:
     password_min_length: 12
     require_special_chars: true
     require_uppercase: true
   ```

2. **Principle of Least Privilege:**
   - Grant minimal necessary permissions
   - Review and rotate regularly

3. **Network Security:**
   ```yaml
   api:
     host: 127.0.0.1  # Localhost only in development
     port: 8080
   ```

   For production with load balancer:
   ```yaml
   api:
     host: 0.0.0.0  # Listen on all interfaces
     port: 8080  # Behind load balancer/firewall
   ```

## Firewall and Network Security

### Firewall Rules

**Development (Local):**
```bash
# Accept only localhost
sudo iptables -I INPUT -p tcp --dport 8080 -i lo -j ACCEPT
sudo iptables -I INPUT -p tcp --dport 8080 -j DROP
```

**Production (With Load Balancer):**
```bash
# Accept from load balancer only
sudo iptables -I INPUT -p tcp --dport 8080 -s 192.168.1.50 -j ACCEPT
sudo iptables -I INPUT -p tcp --dport 8080 -j DROP
```

### Network Isolation

**Docker Network:**
```bash
docker network create fungal-net
docker run --network fungal-net fungal-server
```

**Kubernetes Network Policy:**
```yaml
apiVersion: networking.k8s.io/v1
kind: NetworkPolicy
metadata:
  name: fungal-netpol
spec:
  podSelector:
    matchLabels:
      app: fungal-server
  ingress:
  - from:
    - namespaceSelector:
        matchLabels:
          name: production
  egress:
  - to:
    - namespaceSelector:
        matchLabels:
          name: production
    ports:
    - protocol: TCP
      port: 5432  # Database
```

## Secure Deployment

### Initial Setup Checklist

- [ ] Change default admin password
- [ ] Enable HTTPS/TLS
- [ ] Configure strong authentication method
- [ ] Set up audit logging
- [ ] Enable firewall rules
- [ ] Restrict file permissions
- [ ] Set up backup encryption
- [ ] Test disaster recovery
- [ ] Document security procedures
- [ ] Schedule security review

### Production Hardening

1. **Update System:**
   ```bash
   sudo apt update && sudo apt upgrade
   sudo reboot
   ```

2. **Disable Unnecessary Services:**
   ```bash
   sudo systemctl disable service_name
   sudo systemctl stop service_name
   ```

3. **Enable SSH Key Authentication:**
   ```bash
   ssh-keygen -t ed25519
   ssh-copy-id user@server
   ```

4. **Disable SSH Password Authentication:**
   ```bash
   # /etc/ssh/sshd_config
   PasswordAuthentication no
   ```

5. **Enable UFW Firewall:**
   ```bash
   sudo ufw enable
   sudo ufw default deny incoming
   sudo ufw allow 22/tcp  # SSH
   sudo ufw allow 443/tcp # HTTPS
   ```

### Monitoring for Security

**Check for Failed Logins:**
```bash
grep "Failed password" /var/log/auth.log | wc -l
```

**Monitor Audit Log:**
```bash
tail -f logs/audit.log | grep -E "FAILED|error"
```

**Check File Integrity:**
```bash
# Create baseline
find /app/fungal -type f -exec sha256sum {} \; > baseline.txt

# Verify periodically
find /app/fungal -type f -exec sha256sum {} \; | diff - baseline.txt
```

## Security Incident Response

### Suspected Breach

1. **Immediate Actions:**
   - Stop the system gracefully
   - Preserve logs and audit trail
   - Isolate network

2. **Investigation:**
   - Review audit logs
   - Check for unauthorized access
   - Analyze network traffic logs

3. **Remediation:**
   - Rotate all credentials
   - Patch vulnerabilities
   - Update security policies

4. **Recovery:**
   - Restore from clean backup
   - Verify integrity
   - Test all functionality
   - Resume operations

### Incident Documentation

Log all incidents:
```json
{
  "incident_id": "INC-2023-001",
  "date": "2023-10-03T12:00:00Z",
  "description": "Unauthorized API access attempt",
  "severity": "high",
  "status": "resolved",
  "actions_taken": [
    "Revoked compromised tokens",
    "Updated firewall rules",
    "Reviewed audit logs"
  ]
}
```

## Security Updates

### Check for Vulnerabilities

```bash
# Scan dependencies
cargo audit  # For Rust dependencies
npm audit    # For Node dependencies
```

### Update Procedures

1. **Test in Staging:**
   ```bash
   # Deploy to staging
   make deploy-staging
   
   # Run tests
   make test
   ```

2. **Deploy to Production:**
   ```bash
   # Rolling update (one node at a time)
   make deploy-prod-rolling
   ```

3. **Verify Security:**
   ```bash
   # Verify HTTPS
   openssl s_client -connect localhost:8443 -showcerts
   
   # Verify authentication
   curl -H "Authorization: Bearer invalid_token" http://localhost:8080/api/system/overview
   # Should return 401 Unauthorized
   ```

## Compliance

### Data Protection

**GDPR Compliance:**
- Right to be forgotten: Delete user and related data
- Data portability: Export user data as JSON
- Breach notification: Audit trail of all changes

**HIPAA Compliance (if handling health data):**
- Encrypt all data at rest and in transit
- Implement access controls (RBAC)
- Maintain detailed audit logs
- Regular security assessments

### Regular Audits

**Monthly:**
- Review access logs
- Check user permissions
- Verify backup integrity

**Quarterly:**
- Security assessment
- Penetration testing
- Compliance review

**Annually:**
- Full security audit
- Update security policies
- Train staff

## Support and Reporting

Report security vulnerabilities responsibly:
1. Do not disclose publicly
2. Email security@example.com with details
3. Include proof of concept (if applicable)
4. Allow 90 days for fix before disclosure
