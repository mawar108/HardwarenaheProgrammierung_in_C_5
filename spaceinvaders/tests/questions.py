#!/usr/bin/env python3
import os, re, sys

def check_questions():
    print("INFO: Checking if answers are missing.")
    
    with open("Fragen.txt") as f:
        text = f.read()

    pattern = r"Frage (\d+):(.*?)Antwort (\d+):(.*?)\-+"

    question_numbers = set(map(str, range(1, 11)))

    for match in re.findall(pattern, text, flags=re.DOTALL):
        question_number, question, answer_number, answer = match

        if question_number != answer_number:
            print(f"ERROR: Question {question_number} is followed by answer {answer_number}")
            sys.exit(1)

        if len(answer.strip()) == 0:
            print(f"ERROR: Question {question_number} has not been answered.")
            sys.exit(1)

        print(f"\tOK: Answer to question {question_number} is not empty.")

        if question_number not in question_numbers:
            print(f"ERROR: Question {question_number} should not exist.")
            sys.exit(1)

        question_numbers.remove(question_number)

        if question_number == "10" and len(answer) < 50:
            print("ERROR: The answer for question 10 is too short.")
            print("Question:")
            print(question)
            print("Answer:")
            print(answer)
            sys.exit(1)

    if question_numbers:
        print("ERROR: The following questions could not be found in Fragen.txt:")
        for i in question_numbers:
            print(f"Frage {i}")
        sys.exit(1)

def main():
    check_questions()

    print("\nAlle von uns zur Verfügung gestellten Tests sind durchgelaufen.")
    print("Bitte beachten Sie, dass die Tests nur prüfen, ob die Fragen")
    print("überhaupt beantwortet wurden. Sie dienen nur als Erinnerung.")
    print("Ob die Antworten auch Sinn ergeben prüfen wir erst nach")
    print("der Abgabe.")
    print("Weiterhin wird der Spielablauf nicht automatisch auf Speicher-")
    print("zugriffsfehler getestet. Das ist Ihre Aufgabe.\n\n")

if __name__ == "__main__":
    main()

