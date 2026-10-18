# Security policy

bahguta Homelab is designed for a **trusted local network**: the API has no general authentication (only the endpoints that start and stop Docker containers require an API key) and the backend can control Docker through a restricted proxy (see the security section of the [README](README.md)). Do not expose it to the internet.

## Reporting a vulnerability

Please **do not open a public issue** for a security problem. Report it privately using the repository's *Security → Report a vulnerability* option (GitHub) or by contacting the maintainer directly, and give enough detail to reproduce it. We will answer as soon as possible.

If you find a leaked password, key or personal data in the repository, report it the same way.
