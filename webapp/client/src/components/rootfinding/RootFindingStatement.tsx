import React from 'react';
import { renderKatexInline } from '../../features/katexInline';
import { EQUATIONS } from '../../features/rootfinding/equations';
import type { EqId } from '../../features/rootfinding/types';

export function RootFindingStatement(props: { eq: EqId; loading: boolean }) {
  return (
    <div className="nmGraphMeta">
      <div className="nmGraphStatement">
        <span
          dangerouslySetInnerHTML={{
            __html: renderKatexInline(`${EQUATIONS[props.eq].latex} = 0`),
          }}
        />
      </div>
      {props.loading ? (
        <div className="muted" style={{ fontSize: 13 }}>
          Updating…
        </div>
      ) : null}
    </div>
  );
}
