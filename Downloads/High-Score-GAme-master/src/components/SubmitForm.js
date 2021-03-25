import { Button } from "./Button";
import "./SubmitForm.css";
export default function SubmitForm(props) {
  return (
    <form
      data-testid="submitFrom"
      onSubmit={props.onSubmitAction}
      id="submit-score-form"
    >
      <div className="form__group field">
        <input
          type="input"
          className="form__field"
          placeholder="Name"
          name="name"
          id="name"
          required
          onChange={props.onInputChange}
        />
        <label htmlFor="name" className="form__label">
          Name
        </label>
      </div>
      <Button id="submitBtn" type="submit">
        Submit Score
      </Button>
    </form>
  );
}
