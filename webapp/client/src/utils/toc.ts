export type TocItem = {
  id: string;
  text: string;
  level: 2 | 3;
};

function getHeadingText(h: Element): string {
  // KaTeX outputs both a visible HTML layer and a MathML layer.
  // `textContent` tends to include BOTH, leading to duplicated/ugly ToC entries.
  const clone = h.cloneNode(true) as HTMLElement;
  clone.querySelectorAll('.katex-mathml').forEach((n) => n.remove());
  return (clone.textContent ?? '').replace(/\s+/g, ' ').trim();
}

export function extractTocFromHtml(html: string): TocItem[] {
  if (typeof window === 'undefined') return [];

  const doc = new DOMParser().parseFromString(html, 'text/html');
  const items: TocItem[] = [];

  const headings = doc.querySelectorAll('h2, h3');
  headings.forEach((h) => {
    const level = h.tagName === 'H3' ? 3 : 2;
    const text = getHeadingText(h);
    const id = (h.getAttribute('id') ?? '').trim();
    if (!text || !id) return;
    items.push({ id, text, level });
  });

  return items;
}
