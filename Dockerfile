# syntax=docker/dockerfile:1

# One container that builds:
# - webapp server (TypeScript -> dist)
# - webapp client (Vite -> dist)
# - C++ rootfinding trace binary (Linux) used by /api/rootfinding/trace

FROM node:20-bookworm AS build

RUN apt-get update \
  && apt-get install -y --no-install-recommends g++ make ca-certificates \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Install Node deps (workspace)
COPY webapp/package.json webapp/package-lock.json webapp/README.md webapp/
COPY webapp/client/package.json webapp/client/tsconfig*.json webapp/client/vite.config.* webapp/client/index.html webapp/client/
COPY webapp/server/package.json webapp/server/tsconfig*.json webapp/server/
RUN cd webapp \
  && npm ci

# Copy the rest of the sources needed for builds
COPY webapp/ webapp/
COPY nm-lib/ nm-lib/

# Build server + client
RUN cd webapp \
  && npm run build -w server \
  && npm run build -w client

# Build the C++ test binary needed by the API trace endpoint
RUN mkdir -p nm-lib/bin/tests \
  && g++ -std=c++17 -O2 -Inm-lib -Inm-lib/include -Inm-lib/src \
      -o nm-lib/bin/tests/tema1_rootfinding \
      nm-lib/tests/tema1_rootfinding.cpp \
      nm-lib/src/core/*.cpp \
      nm-lib/src/linear/*.cpp \
      nm-lib/src/nonlinear/*.cpp \
      nm-lib/src/utils/*.cpp


FROM node:20-bookworm-slim AS runtime

ENV NODE_ENV=production
ENV PORT=5180

WORKDIR /app

# Install only runtime Node deps for the server workspace.
# (In npm workspaces, dependencies live in the root lockfile, not per-workspace.)
COPY --from=build /app/webapp/package.json webapp/package.json
COPY --from=build /app/webapp/package-lock.json webapp/package-lock.json
COPY --from=build /app/webapp/server/package.json webapp/server/package.json
RUN cd webapp \
  && npm ci --omit=dev -w server

# Runtime files only
COPY --from=build /app/webapp/server/dist webapp/server/dist
COPY --from=build /app/webapp/server/package.json webapp/server/package.json

COPY --from=build /app/webapp/client/dist webapp/client/dist

COPY --from=build /app/nm-lib/capitole nm-lib/capitole
COPY --from=build /app/nm-lib/bin/tests/tema1_rootfinding nm-lib/bin/tests/tema1_rootfinding

# Static images used by the rendered chapters (served from /images and /chapters/images)
COPY images images

# (Optional) keep these if you later expose /api/pages for more than chapters
# COPY --from=build /app/cursuri cursuri

EXPOSE 5180

CMD ["node", "webapp/server/dist/index.js"]
