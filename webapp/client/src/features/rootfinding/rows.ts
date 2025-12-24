import { formatNumber } from './number';
import type { RootFindingMethodName, RootFindingTraceStep } from './types';

export function rowsFromStep(step: RootFindingTraceStep | undefined, method: RootFindingMethodName): Array<{ k: string; v: string }> {
  if (!step) return [];

  if (method === 'bisection' && 'a' in step) {
    return [
      { k: 'iter', v: String(step.iter) },
      { k: 'a', v: formatNumber(step.a) },
      { k: 'b', v: formatNumber(step.b) },
      { k: 'p', v: formatNumber(step.p) },
      { k: 'f(p)', v: formatNumber(step.fp) },
      { k: 'errorBound', v: formatNumber(step.errorBound ?? NaN) },
    ];
  }

  if (method === 'regulaFalsi' && 'a' in step) {
    return [
      { k: 'iter', v: String(step.iter) },
      { k: 'a', v: formatNumber(step.a) },
      { k: 'b', v: formatNumber(step.b) },
      { k: 'p', v: formatNumber(step.p) },
      { k: 'f(p)', v: formatNumber(step.fp) },
    ];
  }

  if (method === 'secant' && 'x0' in step) {
    return [
      { k: 'iter', v: String(step.iter) },
      { k: 'x0', v: formatNumber(step.x0) },
      { k: 'x1', v: formatNumber(step.x1) },
      { k: 'p', v: formatNumber(step.p) },
      { k: 'f(p)', v: formatNumber(step.fp) },
    ];
  }

  if (method === 'newton' && 'x' in step) {
    return [
      { k: 'iter', v: String(step.iter) },
      { k: 'x', v: formatNumber(step.x) },
      { k: 'f(x)', v: formatNumber(step.fx) },
      { k: "f'(x)", v: formatNumber(step.dfx) },
      { k: 'xNext', v: formatNumber(step.xNext) },
      { k: 'f(xNext)', v: formatNumber(step.fxNext) },
    ];
  }

  return [];
}
