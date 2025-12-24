# Webapp

Browser UI for the Numerical Methods project chapters.

The webapp consists of:
- `webapp/server/`: Node + Express API that serves rendered Markdown as HTML.
- `webapp/client/`: React + Vite client that consumes the API.

The server indexes Markdown pages from a configured allowlist of folders (typically `nm-lib/` and `cursuri/`).

## Run (dev)
From the repository root:

- `cd webapp`
- `npm install`
- `npm run dev`

Default endpoints:
- API: `http://localhost:5180` (override via `PORT`)
- UI: `http://localhost:5173`