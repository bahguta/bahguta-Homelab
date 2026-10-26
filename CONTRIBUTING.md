# Contributing to bahguta Homelab

Thanks for your interest! 🇪🇸 *Puedes escribir issues y pull requests en español o en inglés.*

## Ways to help

- **Report a bug** or ask a question: open an [issue](../../issues) using the templates.
- **Suggest an improvement:** see the [roadmap](docs/ROADMAP.md) for open ideas.
- **Improve the documentation:** fixes, screenshots, build guides and translations are very welcome.
- **Send code:** backend (Java), tablet firmware (C++/LVGL), sensor firmware (C++/PlatformIO) or the web app (Angular/TypeScript).

## Before you start

1. Open an issue (or comment on an existing one) for anything bigger than a small fix, so we can agree on the approach.
2. Fork the repository and create a branch from `main`: `git checkout -b feature/short-description`.
3. Keep each pull request focused on one change.

## Project layout

| Folder | What it is | How to build |
|---|---|---|
| `spring/bahguta-api` | Spring Boot backend (Java 21) | `mvn clean package` |
| `bahguta-display/bahguta_display` | ESP32-P4 tablet firmware (Arduino core 3.1, LVGL) | Arduino IDE / arduino-cli |
| `bahguta-firmware/bahguta-firmware` | ESP32-C3 sensor firmware | `pio run` |
| `bahguta_spa` | Angular web app (PWA) | `npm ci && npm run build` |

## Guidelines

- **Never commit secrets:** no passwords, API keys, WiFi credentials, `.env` or `Config.h`. Use the `*.example` templates and environment variables. Pull requests containing secrets will be asked to rewrite their history.
- **Database changes** go in a new Flyway migration (`V4__...sql`); never edit an applied one.
- **Backend:** follow the existing package structure (`controller`, `service`, `dto`, `mapper`, `entity`, `repository`).
- **Firmware:** keep UI code responsive (no blocking calls in LVGL callbacks) and test on real hardware when you can; say in the PR what you tested on.
- **Web app:** standalone components and signals, as in the existing code. Never put API keys in the app: nginx (production) and `proxy.conf.json` (development) add them.
- **Documentation:** if you change behaviour, config or endpoints, update the README or `docs/`.

## Pull request checklist

- [ ] It builds (`mvn package` / `pio run` / Arduino compile / `npm run build`) for the part you changed.
- [ ] No secrets, personal data or local paths.
- [ ] Docs updated if needed.
- [ ] You described what changed and how you tested it.

## License

By contributing you agree that your contribution is licensed under the [GNU GPL v3.0](LICENSE), the same as the project.
