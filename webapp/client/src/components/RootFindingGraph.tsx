import { useEffect, useMemo, useRef, useState } from 'react';

import { clamp } from '../features/rootfinding/number';
import { rowsFromStep } from '../features/rootfinding/rows';
import { computePlotDerived } from '../features/rootfinding/plotDerived';
import { useRootFindingTrace } from '../features/rootfinding/useRootFindingTrace';
import type { EqId, IntervalId, RootFindingGraphSpec } from '../features/rootfinding/types';

import { RootFindingHeaderControls } from './rootfinding/RootFindingHeaderControls';
import { RootFindingPlot } from './rootfinding/RootFindingPlot';
import { RootFindingParamTable } from './rootfinding/RootFindingParamTable';
import { RootFindingStatement } from './rootfinding/RootFindingStatement';

export type { RootFindingGraphSpec };

export function RootFindingGraph({ spec }: { spec: RootFindingGraphSpec }) {
  const [idx, setIdx] = useState(0);

  // Preset-only params
  const [eq, setEq] = useState<EqId>(spec.eq ?? 4);
  const [interval, setInterval] = useState<IntervalId>(spec.interval ?? 1);

  const rootRef = useRef<HTMLDivElement | null>(null);
  const [isVizLayout, setIsVizLayout] = useState(false);

  useEffect(() => {
    const el = rootRef.current;
    if (!el) return;
    setIsVizLayout(Boolean(el.closest('.markdownWide')));
  }, []);

  const { loading, error, payload } = useRootFindingTrace({ eq, interval });

  useEffect(() => {
    setIdx(0);
  }, [eq, interval, spec.method]);

  const methodTrace = useMemo(() => {
    const m = payload?.methods?.find((mm) => mm.name === spec.method);
    return m?.trace ?? [];
  }, [payload, spec.method]);

  const steps = methodTrace;
  const safeIdx = clamp(idx, 0, Math.max(0, steps.length - 1));
  const step = steps[safeIdx];

  const derived = useMemo(() => {
    if (!payload) return null;
    return computePlotDerived({ eq, payload, method: spec.method });
  }, [payload, eq, spec.method]);

  const title = spec.title ?? `${spec.method} — ecuația ${eq}, intervalul ${interval}`;
  const rows = useMemo(() => rowsFromStep(step, spec.method), [step, spec.method]);

  return (
    <div className="nmWidget" ref={rootRef}>
      <div className="nmWidgetHeader">
        <div className="nmWidgetTitle">{title}</div>
        <RootFindingHeaderControls
          eq={eq}
          interval={interval}
          setEq={setEq}
          setInterval={setInterval}
          iter={steps[safeIdx]?.iter ?? 0}
          canPrev={safeIdx !== 0}
          canNext={steps.length > 0 && safeIdx !== steps.length - 1}
          onPrev={() => setIdx((v) => Math.max(0, v - 1))}
          onNext={() => setIdx((v) => Math.min(Math.max(0, steps.length - 1), v + 1))}
        />
      </div>

      <div className="nmWidgetBody">
        {loading && !payload ? <div className="muted">Loading trace…</div> : null}
        {error ? <div className="error">{error}</div> : null}

        {payload && derived ? (
          <>
            <RootFindingStatement eq={eq} loading={loading} />

            <div className="nmGraphRow">
              <RootFindingPlot
                isVizLayout={isVizLayout}
                derived={derived}
                step={step}
                method={spec.method}
                eq={eq}
                loading={loading}
              />

              <RootFindingParamTable rows={rows} />
            </div>
          </>
        ) : null}
      </div>
    </div>
  );
}
