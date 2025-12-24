import type { EqId, RootFindingMethodName, RootFindingTracePayload } from './types';

export function getAllXValues(eq: EqId, payload: RootFindingTracePayload, method: RootFindingMethodName): number[] {
  const m = payload.methods.find((mm) => mm.name === method);
  const xs: number[] = [payload.a, payload.b];
  if (!m?.trace) return xs;

  for (const step of m.trace) {
    if ('p' in step) xs.push(step.p);
    if ('a' in step) {
      xs.push(step.a);
      xs.push(step.b);
    }
    if ('x0' in step) {
      xs.push(step.x0);
      xs.push(step.x1);
    }
    if ('x' in step) {
      xs.push(step.x);
      xs.push(step.xNext);
    }
  }

  return xs.filter((x) => Number.isFinite(x) && (eq !== 1 || x > 0));
}
