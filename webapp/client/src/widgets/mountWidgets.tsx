import React from 'react';
import ReactDOM from 'react-dom/client';
import { RootFindingGraph, type RootFindingGraphSpec } from '../components/RootFindingGraph';

type WidgetSpec = RootFindingGraphSpec;

function isObject(v: unknown): v is Record<string, unknown> {
  return typeof v === 'object' && v !== null;
}

function parseWidgetJson(text: string): WidgetSpec | null {
  try {
    const parsed: unknown = JSON.parse(text);
    if (!isObject(parsed)) return null;

    if (parsed.type === 'rootfinding-graph') {
      const eq = Number(parsed.eq);
      const interval = Number(parsed.interval);
      const method = typeof parsed.method === 'string' ? parsed.method : '';
      const title = typeof parsed.title === 'string' ? parsed.title : undefined;

      const isEq = eq === 1 || eq === 2 || eq === 3 || eq === 4;
      const isInterval = interval === 1 || interval === 2;
      const isMethod = method === 'bisection' || method === 'regulaFalsi' || method === 'secant' || method === 'newton';
      if (!isEq || !isInterval || !isMethod) return null;

      return { type: 'rootfinding-graph', title, eq, interval, method };
    }

    return null;
  } catch {
    return null;
  }
}

export function mountWidgets(rootEl: HTMLElement): () => void {
  const unmounters: Array<() => void> = [];

  const blocks = rootEl.querySelectorAll('pre > code.language-nm-widget');
  blocks.forEach((codeEl) => {
    const code = codeEl as HTMLElement;
    const raw = code.innerText || code.textContent || '';
    const spec = parseWidgetJson(raw.trim());
    if (!spec) return;

    const pre = code.parentElement;
    if (!pre) return;

    const host = document.createElement('div');
    host.className = 'nmWidgetHost';

    pre.replaceWith(host);

    const reactRoot = ReactDOM.createRoot(host);
    if (spec.type === 'rootfinding-graph') {
      reactRoot.render(<RootFindingGraph spec={spec} />);
    }

    unmounters.push(() => reactRoot.unmount());
  });

  return () => {
    for (const u of unmounters) u();
  };
}
