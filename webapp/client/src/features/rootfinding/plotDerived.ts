import { evalEquation } from './equations';
import { nicePadding } from './number';
import { getAllXValues } from './traceUtils';
import type { EqId, RootFindingMethodName, RootFindingTracePayload } from './types';

export type PlotPoint = { x: number; y: number };

export type PlotDerived = {
  xMin: number;
  xMax: number;
  yMin: number;
  yMax: number;
  pts: PlotPoint[];
};

export function computePlotDerived(params: {
  eq: EqId;
  payload: RootFindingTracePayload;
  method: RootFindingMethodName;
}): PlotDerived {
  const xs = getAllXValues(params.eq, params.payload, params.method);
  let xMin = Math.min(...xs);
  let xMax = Math.max(...xs);
  [xMin, xMax] = nicePadding(xMin, xMax, 0.12);

  if (params.eq === 1) xMin = Math.max(xMin, 1e-6);

  const samples = 520;
  const pts: PlotPoint[] = [];
  for (let i = 0; i < samples; i++) {
    const t = i / (samples - 1);
    const x = xMin + (xMax - xMin) * t;
    const y = evalEquation(params.eq, x);
    if (!Number.isFinite(y)) continue;
    pts.push({ x, y });
  }

  let yMin = Infinity;
  let yMax = -Infinity;
  for (const p of pts) {
    yMin = Math.min(yMin, p.y);
    yMax = Math.max(yMax, p.y);
  }
  if (!Number.isFinite(yMin) || !Number.isFinite(yMax)) {
    yMin = -1;
    yMax = 1;
  }

  ;[yMin, yMax] = nicePadding(yMin, yMax, 0.18);

  return { xMin, xMax, yMin, yMax, pts };
}
