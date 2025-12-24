import fs from 'node:fs/promises';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

import { renderMarkdownToHtml } from './render/markdown';

export type PageInfo = {
  id: string;
  title: string;
  path: string;
};

export type ChapterInfo = {
  id: string;
  title: string;
};

export type ChapterContentHtml = ChapterInfo & {
  html: string;
};

export type PageContent = PageInfo & {
  content: string;
};

export type PageContentHtml = PageInfo & {
  html: string;
};

const THIS_DIR = path.dirname(fileURLToPath(import.meta.url));
const DEFAULT_REPO_ROOT = path.resolve(THIS_DIR, '../../../');
const REPO_ROOT = process.env.CONTENT_ROOT ? path.resolve(process.env.CONTENT_ROOT) : DEFAULT_REPO_ROOT;

const ALLOWED_DIRS = [path.join(REPO_ROOT, 'nm-lib'), path.join(REPO_ROOT, 'cursuri')];

function isWithinAllowed(absPath: string): boolean {
  const normalized = path.normalize(absPath);
  return ALLOWED_DIRS.some((dir) => normalized.startsWith(path.normalize(dir + path.sep)) || normalized === dir);
}

function toId(absPath: string): string {
  // stable, URL-safe-ish id: relative path from repo root, with slashes
  const rel = path.relative(REPO_ROOT, absPath);
  return rel.split(path.sep).join('/');
}

function titleFallbackFromPath(relPath: string): string {
  const base = path.basename(relPath);
  const lower = base.toLowerCase();
  const noExt = lower.endsWith('.md') ? base.slice(0, base.length - 3) : base;
  return noExt.split('_').join(' ').split('-').join(' ').trim();
}

function firstH1Title(markdown: string): string {
  const lines = markdown.split('\n');
  for (const line of lines)
  {
    const t = line.endsWith('\r') ? line.slice(0, line.length - 1).trim() : line.trim();
    if (t.startsWith('# '))
    {
      return t.slice(2).trim();
    }
  }
  return '';
}

function titleFromMarkdownContent(markdown: string, fallbackRelPath: string): string {
  const heading = firstH1Title(markdown);
  if (heading) return heading;
  return titleFallbackFromPath(fallbackRelPath);
}

export async function listMarkdownPages(): Promise<PageInfo[]> {
  const results: PageInfo[] = [];

  async function walk(dirAbs: string): Promise<void> {
    const entries = await fs.readdir(dirAbs, { withFileTypes: true });
    for (const ent of entries) {
      const abs = path.join(dirAbs, ent.name);
      if (!isWithinAllowed(abs)) {
        continue;
      }
      if (ent.isDirectory()) {
        // skip common build/venv folders if they appear
        if (ent.name === 'node_modules' || ent.name === '.git' || ent.name === '.venv') {
          continue;
        }
        await walk(abs);
        continue;
      }
      if (ent.isFile() && ent.name.toLowerCase().endsWith('.md')) {
        const rel = toId(abs);
        const base = path.posix.basename(rel);

        // Vizualizările pentru rootfinding sunt integrate în capitolele 01-04.
        // Păstrăm fișierul vechi în repo (dacă există), dar îl ascundem din listă.
        if (base === '10_vizualizare_radacini.md')
        {
          continue;
        }

        // Fișiere auxiliare pentru PDF (capitole „carte” *_merged.md), nu trebuie listate ca pagini web.
        if (base.endsWith('_merged.md'))
        {
          continue;
        }

        let title = titleFallbackFromPath(rel);
        try {
          const content = await fs.readFile(abs, 'utf-8');
          title = titleFromMarkdownContent(content, rel);
        } catch {
          // fall back to path-derived title
        }

        results.push({ id: rel, title, path: rel });
      }
    }
  }

  for (const dir of ALLOWED_DIRS) {
    try {
      await walk(dir);
    } catch {
      // ignore missing dirs
    }
  }

  function sortKey(p: PageInfo): { dir: string; num: number; weight: number; base: string } {
    const dir = path.posix.dirname(p.path);
    const base = path.posix.basename(p.path);

    // Leading numeric prefix (e.g. "05_gauss...")
    let i = 0;
    while (i < base.length && base[i] >= '0' && base[i] <= '9') i++;
    const num = i > 0 ? Number.parseInt(base.slice(0, i), 10) : Number.MAX_SAFE_INTEGER;

    // Stem (lowercase, without extension and numeric prefix)
    let stem = base;
    if (stem.toLowerCase().endsWith('.md')) stem = stem.slice(0, stem.length - 3);
    let j = 0;
    while (j < stem.length && stem[j] >= '0' && stem[j] <= '9') j++;
    while (j < stem.length && (stem[j] === '_' || stem[j] === '-' || stem[j] === ' ')) j++;
    stem = stem.slice(j).toLowerCase();

    // Keep "theory" first, then "example", then "vizualizare".
    let weight = 0;
    if (stem.includes('teorie')) weight = 1;
    if (stem.includes('exemplu')) weight = 2;
    if (stem.includes('vizualizare')) weight = 3;

    return { dir, num, weight, base: p.path };
  }

  results.sort((a, b) => {
    const ka = sortKey(a);
    const kb = sortKey(b);
    if (ka.dir !== kb.dir) return ka.dir.localeCompare(kb.dir);
    if (ka.num !== kb.num) return ka.num - kb.num;
    if (ka.weight !== kb.weight) return ka.weight - kb.weight;
    return ka.base.localeCompare(kb.base);
  });
  return results;
}

export async function listChapters(prefix?: string): Promise<ChapterInfo[]> {
  const pages = await listMarkdownPages();
  const filtered = prefix ? pages.filter((p) => p.path.startsWith(prefix)) : pages;

  const seen = new Set<string>();
  const chapters: ChapterInfo[] = [];
  const counts = new Map<string, number>();

  for (const p of filtered)
  {
    const baseId = p.title || titleFallbackFromPath(p.path);
    const prev = counts.get(baseId) ?? 0;
    counts.set(baseId, prev + 1);

    let id = baseId;
    if (seen.has(id))
    {
      let k = 2;
      while (seen.has(`${baseId} (${k})`)) k++;
      id = `${baseId} (${k})`;
    }

    if (!id)
    {
      continue;
    }

    seen.add(id);
    chapters.push({ id, title: p.title });
  }

  return chapters;
}

export async function getChapterHtmlBySlug(id: string, prefix?: string): Promise<ChapterContentHtml> {
  const pages = await listMarkdownPages();
  const filtered = prefix ? pages.filter((p) => p.path.startsWith(prefix)) : pages;

  // Recreate the same id assignment as listChapters() so duplicates behave consistently.
  const counts = new Map<string, number>();
  const used = new Set<string>();

  let matchedPage: PageInfo | null = null;

  for (const p of filtered)
  {
    const baseId = p.title || titleFallbackFromPath(p.path);
    const prev = counts.get(baseId) ?? 0;
    counts.set(baseId, prev + 1);

    let computedId = baseId;
    if (used.has(computedId))
    {
      let k = 2;
      while (used.has(`${baseId} (${k})`)) k++;
      computedId = `${baseId} (${k})`;
    }
    used.add(computedId);

    if (computedId === id)
    {
      matchedPage = p;
      break;
    }
  }

  if (!matchedPage)
  {
    throw new Error('Chapter not found');
  }

  const htmlPage = await getMarkdownPageHtml(matchedPage.path);
  return {
    id,
    title: htmlPage.title,
    html: htmlPage.html
  };
}

export async function getMarkdownPage(id: string): Promise<PageContent> {
  const abs = path.join(REPO_ROOT, id.split('/').join(path.sep));
  if (!isWithinAllowed(abs)) {
    throw new Error('Forbidden path');
  }
  const content = await fs.readFile(abs, 'utf-8');
  return {
    id,
    title: titleFromMarkdownContent(content, id),
    path: id,
    content
  };
}

export async function getMarkdownPageHtml(id: string): Promise<PageContentHtml> {
  const page = await getMarkdownPage(id);
  const html = await renderMarkdownToHtml(page.content);
  return {
    id: page.id,
    title: page.title,
    path: page.path,
    html
  };
}
