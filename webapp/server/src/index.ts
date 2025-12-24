import express from 'express';
import cors from 'cors';
import { getChapterHtmlBySlug, getMarkdownPageHtml, listChapters, listMarkdownPages } from './content.js';
import path from 'path';
import fs from 'fs';
import { fileURLToPath } from 'url';
import { execFile } from 'node:child_process';
import { promisify } from 'node:util';

const execFileAsync = promisify(execFile);

const app = express();
app.use(cors());

app.get('/api/health', (_req, res) => {
  res.json({ ok: true });
});

app.get('/api/pages', async (req, res) => {
  try {
    const prefix = typeof req.query.prefix === 'string' ? req.query.prefix : '';
    const pages = await listMarkdownPages();
    const filtered = prefix ? pages.filter((p) => p.path.startsWith(prefix)) : pages;
    res.json(filtered);
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});

app.get('/api/chapters', async (req, res) => {
  try {
    const prefix = typeof req.query.prefix === 'string' ? req.query.prefix : '';
    const chapters = await listChapters(prefix || undefined);
    res.json(chapters);
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});

// Note: the id can contain slashes, hence the (*) matcher.
app.get('/api/pages/:id(*)', async (req, res) => {
  try {
    const raw = req.params.id ?? '';
    const id = decodeURIComponent(raw);
    const page = await getMarkdownPageHtml(id);
    res.json(page);
  } catch (e) {
    res.status(500).json({ error: String(e) });
  }
});

app.get('/api/chapters/:id', async (req, res) => {
  try {
    const raw = req.params.id ?? '';
    const id = decodeURIComponent(raw);
    const prefix = typeof req.query.prefix === 'string' ? req.query.prefix : '';
    const page = await getChapterHtmlBySlug(id, prefix || undefined);
    res.json(page);
  } catch (e) {
    const msg = e instanceof Error ? e.message : String(e);
    if (msg === 'Chapter not found')
    {
      res.status(404).json({ error: 'Chapter not found' });
      return;
    }
    res.status(500).json({ error: msg });
  }
});

app.get('/api/rootfinding/trace', async (req, res) => {
  try {
    const eq = req.query.eq === undefined ? 4 : Number(req.query.eq);
    const interval = req.query.interval === undefined ? 1 : Number(req.query.interval);
    const isEq = eq === 1 || eq === 2 || eq === 3 || eq === 4;
    const isInterval = interval === 1 || interval === 2;
    if (!isEq || !isInterval) {
      res.status(400).json({ error: 'Invalid query. Use ?eq=1..4&interval=1|2' });
      return;
    }

    const aRaw = typeof req.query.a === 'string' ? req.query.a : undefined;
    const bRaw = typeof req.query.b === 'string' ? req.query.b : undefined;
    const x0Raw = typeof req.query.x0 === 'string' ? req.query.x0 : undefined;
    const epsRaw = typeof req.query.eps === 'string' ? req.query.eps : undefined;

    // Locate the compiled C++ test binary that emits the trace JSON.
    // Works for both dev (tsx) and prod (dist) because __dirname is set from import.meta.url.
    const __dirname = path.dirname(fileURLToPath(import.meta.url));
    const repoRoot = path.resolve(__dirname, '../../../');

    const candidates = [
      path.resolve(repoRoot, 'nm-lib/bin/tests/tema1_rootfinding.exe'),
      path.resolve(repoRoot, 'nm-lib/bin/tests/tema1_rootfinding'),
    ];
    const exePath = candidates.find((p) => fs.existsSync(p));
    if (!exePath) {
      res.status(500).json({
        error:
          'Rootfinding binary not found. Build the C++ tests (VS Code task: build (tests)) so nm-lib/bin/tests/tema1_rootfinding(.exe) exists.',
        lookedFor: candidates,
      });
      return;
    }

    const args: string[] = ['--json', '--trace', '--eq', String(eq), '--interval', String(interval)];

    if (aRaw !== undefined || bRaw !== undefined) {
      if (aRaw === undefined || bRaw === undefined) {
        res.status(400).json({ error: 'Provide both a and b, or neither.' });
        return;
      }
      const a = Number(aRaw);
      const b = Number(bRaw);
      if (!Number.isFinite(a) || !Number.isFinite(b)) {
        res.status(400).json({ error: 'Invalid a/b. Must be finite numbers.' });
        return;
      }
      args.push('--a', String(a), '--b', String(b));
    }
    if (x0Raw !== undefined) {
      const x0 = Number(x0Raw);
      if (!Number.isFinite(x0)) {
        res.status(400).json({ error: 'Invalid x0. Must be a finite number.' });
        return;
      }
      args.push('--x0', String(x0));
    }
    if (epsRaw !== undefined) {
      const eps = Number(epsRaw);
      if (!Number.isFinite(eps) || eps <= 0) {
        res.status(400).json({ error: 'Invalid eps. Must be a finite positive number.' });
        return;
      }
      args.push('--eps', String(eps));
    }
    try {
      const { stdout } = await execFileAsync(exePath, args, {
        cwd: repoRoot,
        timeout: 15_000,
        maxBuffer: 5 * 1024 * 1024,
        windowsHide: true,
      });

      const text = String(stdout ?? '').trim();
      const payload = JSON.parse(text);
      res.json(payload);
      return;
    } catch (e: unknown) {
      const errObj = e as { stderr?: unknown; stdout?: unknown; message?: unknown; code?: unknown };
      const stderr = typeof errObj?.stderr === 'string' ? errObj.stderr : '';
      const stdout = typeof errObj?.stdout === 'string' ? errObj.stdout : '';

      const cleaned = (stderr || stdout || '').toString().trim();
      if (cleaned) {
        // Treat as a user/config error (bad interval, invalid eq, etc.).
        res.status(400).json({ error: cleaned.split('\r\n').join('\n') });
        return;
      }

      res.status(500).json({ error: 'Failed to compute rootfinding trace.' });
      return;
    }
  } catch {
    res.status(500).json({ error: 'Failed to handle request.' });
  }
});

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const repoRoot = path.resolve(__dirname, '../../../');
const clientDist = path.resolve(repoRoot, 'webapp/client/dist');
const clientIndex = path.join(clientDist, 'index.html');

if (fs.existsSync(clientIndex))
{
  app.use(express.static(clientDist));

  // SPA history fallback: serve index.html for non-API routes.
  app.get('*', (req, res, next) => {
    if (req.path.startsWith('/api/')) return next();
    res.sendFile(clientIndex);
  });
}

const port = Number(process.env.PORT ?? '5180');
app.listen(port, '0.0.0.0', () => {
  // eslint-disable-next-line no-console
  console.log(`API listening on http://localhost:${port}`);
});
