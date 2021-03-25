import { useState } from "react";
import "./Dropdown.css";
import { Button } from "./Button";

export default function Dropdown(props) {
  return (
    <div className="dropdown">
      <Button id="options">
        options
        <div className="dropdown-options">
          <>
            <a id="option1" onClick={props.selectHighscoreTable}>
              top 10 scoreboard
            </a>
            <a id="option2" onClick={props.selectAvgTable}>
              top 10 avg per click scoreboard
            </a>
          </>
        </div>
      </Button>
    </div>
  );
}
