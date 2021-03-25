import "./HighScoreTable.css";

export default function Highscore(props) {
  const renderData = props.sorted.map((item, pos) => {
    if (props.type == "highscore") {
      return (
        <tr key={pos}>
          <td>{pos + 1}</td>
          <td>{item.name}</td>
          <td>{item.score}</td>
          <td>{item.timesClicked}</td>
        </tr>
      );
    } else {
      return (
        <tr key={pos}>
          <td>{pos + 1}</td>
          <td>{item.name}</td>
          <td>{item.avg}</td>
          <td>{item.timesClicked}</td>
        </tr>
      );
    }
  });

  const renderTableHeaders = props.tableHeaders.map((header, pos) => {
    return <th key={pos}>{header}</th>;
  });

  return (
    <div>
      <h1 className="scoreTitle"> Top 10 scoreboard: </h1>
      <table>
        <thead>
          <tr>{renderTableHeaders}</tr>
        </thead>
        <tbody>{renderData}</tbody>
      </table>
    </div>
  );
}
