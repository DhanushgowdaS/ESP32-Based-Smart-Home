# Smart Home Dashboard

Responsive dark-theme dashboard based on the supplied mobile reference.

## Local preview

Open `index.html` in a browser, or serve this folder with a static web server. Without an ESP32 connected, preview mode supports appliance toggles and editing each channel's name, type, icon, and color. Settings are stored in browser localStorage.

## Planned firmware API contract

The dashboard connection adapter expects these endpoints from a later firmware implementation:

- `GET /api/status` → `{"channels":[false,true,false,false]}`
- `POST /api/relay/{1..4}` with `{"state":true}`
- WebSocket `/ws` → `{"channel":2,"state":true}`

These are interface expectations, not a claim that the integrated firmware is already implemented. The firmware and dashboard contract must be validated together.

## LittleFS deployment

The contents of this dashboard directory will be copied to the ESP32 LittleFS image in a later integration stage. Keep this directory as the maintained source.