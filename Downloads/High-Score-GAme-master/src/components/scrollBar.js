import React from "react";
import "./scrollBar.css";
import arrow from "../assets/arrow.png";
import illustration from "../assets/illustration.png";

export default function ScrollBAr() {
  const scroll = () => {
    window.scrollTo(0, 800);
  };
  return (
    <div id="parent">
      <div id="bluewallpaper"></div>
      <div className="center">
        <img id="illustration" src={illustration} />
        <div className="effect">
          High Score Game
          <div>High Score Game</div>
          <div>High Score Game</div>
          <div>High Score Game</div>
          <div>High Score Game</div>
        </div>
      </div>
      <section className="main">
        <img src={arrow} className="arrow-wrap" onClick={scroll} />
      </section>
    </div>
  );
}
