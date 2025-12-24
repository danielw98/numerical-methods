import type { TocItem } from '../utils/toc';

export function TableOfContents({ items }: { items: TocItem[] }) {
  if (items.length === 0) return null;

  function onClick(id: string) {
    const el = document.getElementById(id);
    if (!el) return;
    const marginTop = 80;
    const rect = el.getBoundingClientRect();
    const absoluteTop = window.scrollY + rect.top - marginTop;
    window.scrollTo({ top: absoluteTop, behavior: 'smooth' });

    // Preserve the anchor in the URL without navigating (and keep current ?page=...).
    const url = new URL(window.location.href);
    url.hash = id;
    history.replaceState(null, '', url.toString());
  }

  return (
    <nav className="toc" aria-label="Table of contents">
      <div className="tocTitle">Contents</div>
      <ul className="tocList">
        {items.map((it) => (
          <li key={it.id} className={it.level === 3 ? 'tocItem tocItemL3' : 'tocItem'}>
            <button className="tocLink" type="button" onClick={() => onClick(it.id)}>
              {it.text}
            </button>
          </li>
        ))}
      </ul>
    </nav>
  );
}
