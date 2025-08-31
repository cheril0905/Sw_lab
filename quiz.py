import random
import matplotlib.pyplot as plt

# List of 10 questions and answers
questions = [
    "What is the capital of France?",
    "What is 5 + 7",
    "What is the boiling point of water in Celsius?",
    "Who wrote 'Hamlet'?",
    "What is the largest planet in our solar system?",
    "Which year did World War II end?",
    "What is the chemical symbol for Gold?",
    "Who painted the Mona Lisa?",
    "What is the square root of 64?",
    "Which continent is the Sahara Desert located on?"
]

answers = [
    "paris",
    "12",
    "100",
    "shakespeare",
    "jupiter",
    "1945",
    "au",
    "da vinci",
    "8",
    "africa"
]

# Select 4 random questions
selected_indices = random.sample(range(10), 4)
score = 0
results = []

for i in selected_indices:
    print(questions[i])
    
    # Take answer from player in terminal
    user_answer = input("Your answer: ").strip().lower()
    
    if user_answer == answers[i]:
        print("✅ Correct!\n")
        score += 1
        results.append(1)
    else:
        print(f"❌ Wrong! Correct answer: {answers[i].title()}\n")
        results.append(0)

print(f"You got {score} out of 4 correct.")

# Visualization
labels = ['Correct', 'Incorrect']
counts = [results.count(1), results.count(0)]

plt.bar(labels, counts, color=['green', 'red'])
plt.title("Quiz Results")
plt.ylabel("Number of Questions")
plt.show()
