import React from 'react';

export function RootFindingParamTable(props: { rows: Array<{ k: string; v: string }> }) {
  if (!props.rows.length) return null;

  return (
    <table className="nmWidgetTable">
      <thead>
        <tr>
          <th>param</th>
          <th>value</th>
        </tr>
      </thead>
      <tbody>
        {props.rows.map((r) => (
          <tr key={r.k}>
            <td>{r.k}</td>
            <td>{r.v}</td>
          </tr>
        ))}
      </tbody>
    </table>
  );
}
