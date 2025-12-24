export type RootFindingMethodName = 'bisection' | 'regulaFalsi' | 'secant' | 'newton';

export type EqId = 1 | 2 | 3 | 4;
export type IntervalId = 1 | 2;

export type RootFindingTraceStep =
  | { iter: number; a: number; b: number; p: number; fp: number; errorBound?: number }
  | { iter: number; x0: number; x1: number; p: number; fp: number }
  | { iter: number; x: number; fx: number; dfx: number; xNext: number; fxNext: number };

export type RootFindingMethod = {
  name: RootFindingMethodName;
  trace?: RootFindingTraceStep[];
};

export type RootFindingTracePayload = {
  kind: 'rootfinding';
  eq: number;
  interval: number;
  eps: number;
  statement: string;
  a: number;
  b: number;
  x0?: number;
  traceEnabled: boolean;
  methods: RootFindingMethod[];
};

export type RootFindingGraphSpec = {
  type: 'rootfinding-graph';
  title?: string;
  eq: EqId;
  interval: IntervalId;
  method: RootFindingMethodName;
};
