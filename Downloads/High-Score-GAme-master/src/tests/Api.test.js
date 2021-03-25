import React from "react";
import ReactDom from 'react-dom';
import {fireEvent, render} from '@testing-library/react';
import Game from "../components/Game";

it("render without crashing" ,() => {
    const div = document.createElement("Div");
    ReactDom.render(<Game></Game>, div);
    expect(div.querySelector("h1").textContent).toBe("Current Score: 0");
})

test("allow user to add his name and score" , ()=>{
    const { getByLabelText, queryByText } = render(<Game></Game>);
    const div = document.createElement("Div");
    ReactDom.render(<Game></Game>, div);
    const input = getByLabelText("Name");
    fireEvent.change(input, { target : "new name"});
    fireEvent.click(queryByText("Add"));
    fireEvent.click(queryByText("Submit Score")); 
    expect(div.querySelector("h1").textContent).toBe("Current Score: 0");

})

