import type { EqId, IntervalId, RootFindingTracePayload } from './types';

export function buildRootFindingTraceUrl(params: { eq: EqId; interval: IntervalId }): string {
  const qs = new URLSearchParams();
  qs.set('eq', String(params.eq));
  qs.set('interval', String(params.interval));
  return `/api/rootfinding/trace?${qs.toString()}`;
}

export async function fetchRootFindingTrace(params: {
  url: string;
  signal: AbortSignal;
}): Promise<RootFindingTracePayload> {
  const res = await fetch(params.url, { signal: params.signal });
  const bodyText = await res.text();

  if (!res.ok) {
    try {
      const j = JSON.parse(bodyText) as { error?: string };
      const msg = typeof j?.error === 'string' ? j.error : bodyText;
      throw new Error(msg);
    } catch {
      throw new Error(bodyText);
    }
  }

  return JSON.parse(bodyText) as RootFindingTracePayload;
}
