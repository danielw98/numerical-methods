import { useEffect, useMemo, useState } from 'react';
import { getChapterHtml, listChapters } from './api';
import { TableOfContents } from './components/TableOfContents';
import type { ChapterContentHtml, ChapterInfo } from './types';
import { extractTocFromHtml } from './utils/toc';
import { mountWidgets } from './widgets/mountWidgets';

const CHAPTER_PATH_PREFIX = '/chapters/';

function getSelectedIdFromLocation(): string {
  const path = window.location.pathname;
  if (path.startsWith(CHAPTER_PATH_PREFIX))
  {
    const raw = path.slice(CHAPTER_PATH_PREFIX.length);
    try {
      return decodeURIComponent(raw);
    } catch {
      return raw;
    }
  }

  return '';
}

function setSelectedIdInLocation(id: string, mode: 'push' | 'replace') {
  const url = new URL(window.location.href);
  url.pathname = `${CHAPTER_PATH_PREFIX}${encodeURIComponent(id)}`;
  url.search = '';
  // If the previous chapter had a #anchor (from ToC), it would otherwise persist.
  // Anchors are page-specific, so we clear it on chapter navigation.
  url.hash = '';
  if (mode === 'push')
  {
    history.pushState(null, '', url.toString());
  }
  else
  {
    history.replaceState(null, '', url.toString());
  }
}

function tryGetIdFromChapterHref(href: string): string | null {
  if (!href.startsWith(CHAPTER_PATH_PREFIX)) return null;
  const raw = href.slice(CHAPTER_PATH_PREFIX.length);
  const withoutHash = raw.split('#')[0];
  const withoutQuery = withoutHash.split('?')[0];
  const cleaned = withoutQuery.split('/').filter(Boolean).join('/');
  if (!cleaned) return null;
  try {
    return decodeURIComponent(cleaned);
  } catch {
    return cleaned;
  }
}

function scrollToHeadingId(id: string) {
  const el = document.getElementById(id);
  if (!el) return;

  const scrollContainer = document.querySelector('.content');
  const marginTop = 12;

  if (scrollContainer instanceof HTMLElement)
  {
    const containerRect = scrollContainer.getBoundingClientRect();
    const elRect = el.getBoundingClientRect();
    const targetTop = elRect.top - containerRect.top + scrollContainer.scrollTop - marginTop;
    scrollContainer.scrollTo({ top: targetTop, behavior: 'smooth' });
  }
  else
  {
    el.scrollIntoView({ behavior: 'smooth', block: 'start' });
  }
}

export default function App() {
  const [chapters, setChapters] = useState<ChapterInfo[]>([]);
  const [selectedId, setSelectedId] = useState<string>('');
  const [chapter, setChapter] = useState<ChapterContentHtml | null>(null);
  const [pagesError, setPagesError] = useState<string>('');
  const [pagesLoading, setPagesLoading] = useState<boolean>(false);

  useEffect(() => {
    let mounted = true;
    setPagesLoading(true);
    setPagesError('');
    listChapters('nm-lib/capitole/')
      .then((list) => {
        if (!mounted) return;
        setChapters(list);
        setSelectedId((prev) => {
          if (prev) return prev;
          const fromLocation = getSelectedIdFromLocation();
          if (fromLocation && list.some((p) => p.id === fromLocation))
          {
            return fromLocation;
          }
          return list[0]?.id ?? '';
        });
      })
      .catch((e: unknown) => {
        if (!mounted) return;
        setPagesError(String(e));
      })
      .finally(() => {
        if (!mounted) return;
        setPagesLoading(false);
      });
    return () => {
      mounted = false;
    };
  }, []);

  useEffect(() => {
    // Enable back/forward navigation.
    function onPopState() {
      const fromLocation = getSelectedIdFromLocation();
      if (!fromLocation) return;
      if (chapters.some((p) => p.id === fromLocation))
      {
        setSelectedId(fromLocation);
      }
    }

    window.addEventListener('popstate', onPopState);
    return () => {
      window.removeEventListener('popstate', onPopState);
    };
  }, [chapters]);

  useEffect(() => {
    if (!selectedId) {
      setChapter(null);
      return;
    }
    let mounted = true;
    setPagesError('');
    getChapterHtml(selectedId, 'nm-lib/capitole/')
      .then((p) => {
        if (!mounted) return;
        setChapter(p);
      })
      .catch((e: unknown) => {
        if (!mounted) return;
        setPagesError(String(e));
        setChapter(null);
      });
    return () => {
      mounted = false;
    };
  }, [selectedId]);

  useEffect(() => {
    if (!selectedId) return;
    // Keep the URL in sync with the selected chapter (shareable, path-style).
    // If we're not already on /chapters/..., move there.
    if (!window.location.pathname.startsWith(CHAPTER_PATH_PREFIX))
    {
      setSelectedIdInLocation(selectedId, 'replace');
    }
  }, [selectedId]);

  useEffect(() => {
    // If the URL contains a hash, scroll to that heading inside the nested scroller
    // after the page HTML is mounted.
    if (!chapter) return;
    const raw = window.location.hash.startsWith('#') ? window.location.hash.slice(1) : '';
    if (!raw) return;
    const id = decodeURIComponent(raw);
    // Wait a tick for DOM to reflect the new HTML.
    const t = window.setTimeout(() => scrollToHeadingId(id), 0);
    return () => window.clearTimeout(t);
  }, [chapter]);

  const sidebar = useMemo(() => {
    if (pagesLoading) return <div className="muted">Loading…</div>;
    if (chapters.length === 0) return <div className="muted">No chapters found.</div>;

    const getGroupTitle = (p: ChapterInfo): string => {
      const t = (p.title || '').toLowerCase();

      const isRootfinding =
        t.includes('bisec') ||
        t.includes('regula falsi') ||
        t.includes('secant') ||
        (t.includes('newton') && !t.includes('sisteme'));

      if (isRootfinding) return 'Aproximarea soluțiilor ecuațiilor';

      const isNonlinearSystems =
        t.includes('neliniare') ||
        (t.includes('newton') && t.includes('sisteme'));

      if (isNonlinearSystems) return 'Sisteme de ecuații neliniare';

      const isLinearSystems =
        t.includes('gauss') ||
        t.includes('jacobi') ||
        t.includes('seidel') ||
        (t.includes('sisteme') && t.includes('liniare'));

      if (isLinearSystems) return 'Sisteme de ecuații liniare';

      return 'Altele';
    };

    const groupOrder: string[] = [];
    const byTitle = new Map<string, ChapterInfo[]>();

    for (const p of chapters)
    {
      const title = getGroupTitle(p);
      if (!byTitle.has(title))
      {
        byTitle.set(title, []);
        groupOrder.push(title);
      }
      byTitle.get(title)!.push(p);
    }

    const groups: Array<{ title: string; pages: ChapterInfo[] }> = groupOrder.map((title) => ({
      title,
      pages: byTitle.get(title) ?? [],
    }));

    return (
      <div className="pageGroups">
        {groups.map((g) => (
          <section className="pageGroup" key={g.title}>
            <div className="pageGroupTitle">{g.title}</div>
            <ul className="pageList">
              {g.pages.map((p) => (
                <li key={p.id}>
                  <button
                    className={p.id === selectedId ? 'pageBtn active' : 'pageBtn'}
                    onClick={() => {
                      setSelectedIdInLocation(p.id, 'push');
                      setSelectedId(p.id);
                    }}
                    title={p.title}
                  >
                    {p.title}
                  </button>
                </li>
              ))}
            </ul>
          </section>
        ))}
      </div>
    );
  }, [pagesLoading, chapters, selectedId]);

  const selectedTitle = useMemo(() => {
    if (!selectedId) return '';
    return chapters.find((p) => p.id === selectedId)?.title ?? selectedId;
  }, [chapters, selectedId]);

  const toc = useMemo(() => {
    if (!chapter) return [];
    return extractTocFromHtml(chapter.html);
  }, [chapter]);

  const isVizualizarePage = useMemo(() => {
    const s = selectedId.toLowerCase();
    return s.includes('vizualizare');
  }, [selectedId]);

  useEffect(() => {
    if (!chapter) return;
    const el = document.querySelector('.markdown');
    if (!(el instanceof HTMLElement)) return;
    return mountWidgets(el);
  }, [chapter]);

  useEffect(() => {
    // SPA navigation for internal chapter links embedded in markdown.
    const container = document.querySelector('.contentCard');
    if (!(container instanceof HTMLElement)) return;

    const onClick = (e: MouseEvent) => {
      const t = e.target;
      if (!(t instanceof Element)) return;
      const a = t.closest('a');
      if (!(a instanceof HTMLAnchorElement)) return;

      const href = a.getAttribute('href') ?? '';
      // Allow normal browser behavior for in-page anchors and external links.
      if (!href || href.startsWith('#') || href.startsWith('http:') || href.startsWith('https:') || href.startsWith('mailto:')) return;

      const id = tryGetIdFromChapterHref(href);
      if (!id) return;
      if (!chapters.some((p) => p.id === id)) return;

      e.preventDefault();
      setSelectedIdInLocation(id, 'push');
      setSelectedId(id);
    };

    container.addEventListener('click', onClick);
    return () => container.removeEventListener('click', onClick);
  }, [chapters]);

  return (
    <div className="appShell">
      <header className="navbar">
        <div className="navbarBrand">Metode numerice</div>
      </header>

      <div className="layout">
        <aside className="sidebar">
          <div className="sidebarHeader">Chapters</div>
          {sidebar}
        </aside>
        <main className="content">
          <div className="contentInner">
            <div className="contentCard">
              {!isVizualizarePage ? (
                <div className="pageMeta">
                  <div className="pageTitle">{selectedTitle || 'Select a chapter.'}</div>
                </div>
              ) : null}

              {pagesError ? <div className="error">{pagesError}</div> : null}
              {!chapter ? <div className="muted">Select a chapter.</div> : null}
              {chapter && !isVizualizarePage ? <TableOfContents items={toc} /> : null}
              {chapter ? (
                <div
                  className={isVizualizarePage ? 'markdown markdownWide' : 'markdown'}
                  dangerouslySetInnerHTML={{ __html: chapter.html }}
                />
              ) : null}
            </div>
          </div>
        </main>
      </div>
    </div>
  );
}
