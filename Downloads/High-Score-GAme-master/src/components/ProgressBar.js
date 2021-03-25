import React from "react";

const ProgressBar = (props) => {
  const { tries } = props;

  const containerStyles = {
    height: 20,
    width: "80%",
    backgroundColor: "#DCECF8",
    borderRadius: 50,
    marginLeft: "10%",
    marginTop: 30,
    marginBottom: 30,
  };

  const fillerStyles = {
    height: "100%",
    width: `${tries * 10}%`,
    backgroundColor: "#366F9A",
    borderRadius: "inherit",
    textAlign: "right",
  };

  const labelStyles = {
    padding: 5,
    color: "white",
    fontWeight: "bold",
  };

  return (
    <div style={containerStyles}>
      <div style={fillerStyles}>
        <span style={labelStyles}>{tries}</span>
      </div>
    </div>
  );
};

export default ProgressBar;
