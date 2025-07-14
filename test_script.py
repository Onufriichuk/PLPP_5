def generate_cases():
    case_list = [
        ("7 - 2 * 3", "1"),
        ("12 / 2 + 1", "7"),
        ("4 + 5 * 6", "34"),
        ("18 / (2 + 1)", "6"),
        ("3 + 4 * 2 - 1", "10"),
        ("(8 - 3) * (2 + 1)", "15"),
        ("9 + 6 / 2", "12"),
        ("14 - 3 * 2 + 1", "9"),
        ("7.5 + 2.5 * 2", "12.5"),
        ("(10.0 - 2.0) / 4", "2")
    ]

    with open("test_input.txt", "w") as file:
        for expr, _ in case_list:
            file.write(expr + "\n")
        file.write("quit\n")

    with open("expected_output.txt", "w") as file:
        file.write("Expected Output:\n")
        file.write("====================\n")
        for expr, expected in case_list:
            file.write(f"{expr} = {expected}\n")

    print("✔ test_input.txt ready")
    print("✔ expected_output.txt ready")
    print("== TO EXECUTE TESTS ==")
    print("1. Compile: g++ -o run_test main.cpp")
    print("2. Run: ./run_test < test_input.txt")
    print("3. Check output against expected_output.txt")


if __name__ == "__main__":
    generate_cases()
