import type { EqId, IntervalId } from './types';

export const EQUATIONS: Record<EqId, { short: string; latex: string }> = {
  1: { short: 'x^2 - 4x + 4 - ln(x)', latex: 'x^2 - 4x + 4 - \\ln(x)' },
  2: { short: 'x + 1 - 2 sin(πx)', latex: 'x + 1 - 2\\sin(\\pi x)' },
  3: { short: 'e^x - 3x^2', latex: 'e^x - 3x^2' },
  4: { short: '2x cos(2x) - (x-2)^2', latex: '2x\\cos(2x) - (x-2)^2' },
};

export function clampEq(v: number): EqId {
  if (v === 1 || v === 2 || v === 3 || v === 4) return v;
  return 4;
}

export function clampInterval(v: number): IntervalId {
  if (v === 1 || v === 2) return v;
  return 1;
}

export function evalEquation(eq: EqId, x: number): number {
  switch (eq) {
    case 1:
      if (x <= 0) return Number.NaN;
      return x * x - 4 * x + 4 - Math.log(x);
    case 2: {
      const pi = Math.PI;
      return x + 1 - 2 * Math.sin(pi * x);
    }
    case 3:
      return Math.exp(x) - 3 * x * x;
    case 4:
      return 2 * x * Math.cos(2 * x) - (x - 2) * (x - 2);
  }
}
