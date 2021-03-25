const sortHighScores = (scoreArray) => {
  let sortedScoreArray = [...scoreArray];
  return sortedScoreArray
    .sort((a, b) =>
      a.score > b.score
        ? -1
        : a.score === b.score
        ? a.timesClicked > b.timesClicked
          ? -1
          : 1
        : 1
    )
    .slice(0, 10);
};

const sortAvgScores = (scoreArray) => {
  let sortedScoreArray = [...scoreArray];
  return sortedScoreArray
    .sort((a, b) =>
      a.avg > b.avg
        ? -1
        : a.avg === b.avg
        ? a.timesClicked > b.timesClicked
          ? -1
          : 1
        : 1
    )
    .slice(0, 10);
};

export { sortHighScores, sortAvgScores };
