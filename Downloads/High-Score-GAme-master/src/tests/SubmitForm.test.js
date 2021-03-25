import React from 'react';
import ReactDom from 'react-dom';
import SubmitForm from "../components/SubmitForm";
import {render } from '@testing-library/react';
import "@testing-library/jest-dom/extend-expect";

it("render without crashing" ,() => {
    const div = document.createElement("Div");
    ReactDom.render(<SubmitForm></SubmitForm>, div);
} )



