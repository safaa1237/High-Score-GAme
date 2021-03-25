import { useState, useEffect } from "react";
import Dropdown from "../components/Dropdown";
import Highscore from "../components/HighScoreTable";
import SubmitForm from "../components/SubmitForm";
import Tries from "../components/ProgressBar";
import { Button } from "../components/Button";
import dices from "../assets/dices.png";
import "./Game.css";
import { sortHighScores, sortAvgScores } from "../utils/Sort";

export default function Game() {
  const [score, setScore] = useState(0);
  const [count, setCount] = useState(0);
  const [name, setName] = useState("");
  const [data, setData] = useState([]);
  const [timesPlayed, setTimesPlayed] = useState(0);
  const [selectedTable, setSelectedTable] = useState("highscore");
  const highscoreTableHeaders = ["Position", "Name", "Score", "Times Clicked"];
  const avgTableHeaders = [
    "Position",
    "Name",
    "Avg Score per Click",
    "Times Clicked",
  ];

  const onBtnIncrementCounter = () => {
    let points = Math.floor(Math.random() * (100 + 100) - 100);
    setScore(score + points);
    setCount(count + 1);
  };

  const resetGame = () => {
    setTimesPlayed(timesPlayed + 1);
    setScore(0);
    setCount(0);
  };

  const handleChange = (e) => {
    setName(e.target.value);
  };

  //the api pranater that  i added is avg , in order to make the rendeing lighter on the front end part , after each
  //submt the abg score is calculated
  const handleSubmit = (e) => {
    fetch("http://localhost:5000/scores", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        name: name,
        score: score,
        timesClicked: count,
        avg: Math.round(score / count),
      }),
    }).then((response) => {
      console.log(response);
      return response.json();
    });
    e.preventDefault();
    e.target[0].value = "";
    resetGame();
  };
  //the parameters to add to the api would be a variable avg , rather then counter the avg each time the user toggles to

  useEffect(() => {
    fetch("http://localhost:5000/scores")
      .then()
      .then((response) => response.json())
      .then((data) => {
        setData(data);
      });
  }, [timesPlayed]);

  const onBtnClickSelectTable = (tableName) => {
    setSelectedTable(tableName);
  };

  return (
    <div className="container">
      <div className="game-container">
        <img id="dices" src={dices} />
        <div className="container">
          <h1 className="score">Current Score: {score}</h1>
          <Button
            {...(count == 10 ? { buttonStyle: "btn--disabled" } : {})}
            id="addButton"
            onClick={onBtnIncrementCounter}
          >
            Add
          </Button>
          {count === 10 ? (
            <div class="alert">You have run out of tries</div>
          ) : null}
        </div>
        <Tries tries={10 - count}></Tries>
        <SubmitForm
          data-testid="submit"
          onSubmitAction={handleSubmit}
          onInputChange={handleChange}
        />
      </div>

      <div className="highscore-table">
        <Dropdown
          selectHighscoreTable={() => onBtnClickSelectTable("highscore")}
          selectAvgTable={() => onBtnClickSelectTable("avg")}
        />
        {selectedTable === "highscore" ? (
          <Highscore
            sorted={sortHighScores(data)}
            tableHeaders={highscoreTableHeaders}
            type="highscore"
          />
        ) : null}
        {selectedTable === "avg" ? (
          <Highscore
            sorted={sortAvgScores(data)}
            tableHeaders={avgTableHeaders}
            type="average"
          />
        ) : null}
      </div>
    </div>
  );
}
