import { useEffect, useMemo, useRef, useState } from 'react';
import { buildRootFindingTraceUrl, fetchRootFindingTrace } from './api';
import type { EqId, IntervalId, RootFindingTracePayload } from './types';

export function useRootFindingTrace(params: { eq: EqId; interval: IntervalId }) {
  const requestUrl = useMemo(() => buildRootFindingTraceUrl(params), [params.eq, params.interval]);

  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string>('');
  const [payload, setPayload] = useState<RootFindingTracePayload | null>(null);

  const abortRef = useRef<AbortController | null>(null);

  useEffect(() => {
    const ac = new AbortController();
    abortRef.current?.abort();
    abortRef.current = ac;

    setLoading(true);
    setError('');

    fetchRootFindingTrace({ url: requestUrl, signal: ac.signal })
      .then((j) => {
        setPayload(j);
      })
      .catch((e: unknown) => {
        if (ac.signal.aborted) return;
        setError(e instanceof Error ? e.message : String(e));
      })
      .finally(() => {
        if (ac.signal.aborted) return;
        setLoading(false);
      });

    return () => {
      ac.abort();
    };
  }, [requestUrl]);

  return { requestUrl, loading, error, payload };
}
