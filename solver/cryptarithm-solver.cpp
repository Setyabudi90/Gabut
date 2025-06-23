#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <regex>
#include <cctype>
#include <thread>

// Cross-platform includes
#ifdef _WIN32
    #include <windows.h>
    #define ENABLE_COLORS true
#else
    #include <unistd.h>
    #define ENABLE_COLORS (isatty(STDOUT_FILENO))
#endif

class CryptarithmSolver {
private:
    struct Term {
        std::string word;
        int coefficient;
        int exponent;
        std::string operation; // For future math operations
        Term(const std::string& w, int c = 1, int e = 1) : word(w), coefficient(c), exponent(e) {}
    };
    
    std::vector<Term> leftTerms;
    std::vector<Term> rightTerms;
    std::unordered_set<char> letters;
    std::unordered_set<char> leadingLetters;
    std::unordered_map<char, int> assignment;
    std::vector<char> letterOrder;
    std::vector<bool> usedDigits;
    std::string lastError;
    int solutionCount;
    bool showAllSolutions;
    
    // Enhanced UI Colors with better error handling
    void setColor(int color) {
        if (!ENABLE_COLORS) return;
        
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, color);
        }
#else
        std::string colorCode;
        switch(color) {
            case 10: colorCode = "\033[92m"; break; // Bright Green
            case 12: colorCode = "\033[91m"; break; // Bright Red  
            case 14: colorCode = "\033[93m"; break; // Bright Yellow
            case 11: colorCode = "\033[96m"; break; // Bright Cyan
            case 13: colorCode = "\033[95m"; break; // Bright Magenta
            case 15: colorCode = "\033[97m"; break; // Bright White
            case 8:  colorCode = "\033[90m"; break; // Dark Gray
            case 9:  colorCode = "\033[94m"; break; // Bright Blue
            case 6:  colorCode = "\033[36m"; break; // Dark Cyan
            case 4:  colorCode = "\033[34m"; break; // Dark Blue
            case 2:  colorCode = "\033[32m"; break; // Dark Green
            default: colorCode = "\033[0m"; break;  // Reset
        }
        std::cout << colorCode;
#endif
    }
    
    void resetColor() {
        if (!ENABLE_COLORS) return;
        
#ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleTextAttribute(hConsole, 7);
        }
#else
        std::cout << "\033[0m";
#endif
    }
    
    void printAnimatedHeader() {
        setColor(11); // Bright Cyan
        std::cout << "\n";
        std::cout << "╔══════════════════════════════════════════════════════════════════════╗\n";
        setColor(14); // Bright Yellow
        std::cout << "║";
        setColor(13); // Bright Magenta
        std::cout << "              🧮✨ ENHANCED CRYPTARITHM SOLVER ✨🧮              ";
        setColor(14); // Bright Yellow
        std::cout << "║\n";
        setColor(11); // Bright Cyan
        std::cout << "║";
        setColor(15); // Bright White
        std::cout << "            Advanced Mathematical Puzzle Solver v2.0             ";
        setColor(11); // Bright Cyan
        std::cout << "║\n";
        std::cout << "║";
        setColor(10); // Bright Green
        std::cout << "        🔢 Multi-operations 🧠 Smart Solving 🎨 Rich UI         ";
        setColor(11); // Bright Cyan
        std::cout << "║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════════════╝\n";
        resetColor();
    }
    
    void printStyledSeparator(char symbol = '-', int color = 8) {
        setColor(color);
        for(int i = 0; i < 70; i++) std::cout << symbol;
        std::cout << "\n";
        resetColor();
    }
    
    void printGradientBox(const std::string& title, const std::string& content, int titleColor = 14, int borderColor = 11) {
        setColor(borderColor);
        std::cout << "┌─ ";
        setColor(titleColor);
        std::cout << "🎯 " << title << " ";
        setColor(borderColor);
        for(size_t i = 0; i < 60 - title.length(); i++) std::cout << "─";
        std::cout << "┐\n";
        resetColor();
        
        std::istringstream iss(content);
        std::string line;
        while(std::getline(iss, line)) {
            setColor(borderColor);
            std::cout << "│ ";
            setColor(15); // Bright White
            std::cout << std::left << std::setw(62) << line;
            setColor(borderColor);
            std::cout << "│\n";
        }
        
        setColor(borderColor);
        std::cout << "└";
        for(int i = 0; i < 64; i++) std::cout << "─";
        std::cout << "┘\n";
        resetColor();
    }
    
    void printErrorBox(const std::string& errorMsg) {
        setColor(12); // Bright Red
        std::cout << "\n╔";
        for(int i = 0; i < 66; i++) std::cout << "═";
        std::cout << "╗\n";
        
        std::cout << "║ ";
        setColor(15); // Bright White
        std::cout << "❌ ERROR: ";
        setColor(14); // Bright Yellow
        std::cout << std::left << std::setw(54) << errorMsg;
        setColor(12); // Bright Red
        std::cout << " ║\n";
        
        std::cout << "╚";
        for(int i = 0; i < 66; i++) std::cout << "═";
        std::cout << "╝\n";
        resetColor();
    }
    
    void printSuccessBox(const std::string& message) {
        setColor(10); // Bright Green
        std::cout << "\n╔";
        for(int i = 0; i < 66; i++) std::cout << "═";
        std::cout << "╗\n";
        
        std::cout << "║ ";
        setColor(15); // Bright White
        std::cout << "✅ SUCCESS: ";
        setColor(14); // Bright Yellow
        std::cout << std::left << std::setw(52) << message;
        setColor(10); // Bright Green
        std::cout << " ║\n";
        
        std::cout << "╚";
        for(int i = 0; i < 66; i++) std::cout << "═";
        std::cout << "╝\n";
        resetColor();
    }
    
    void printWarningBox(const std::string& warning) {
        setColor(14); // Bright Yellow
        std::cout << "\n⚠️  ";
        setColor(15); // Bright White
        std::cout << "WARNING: ";
        setColor(14); // Bright Yellow
        std::cout << warning << "\n";
        resetColor();
    }
    
    void printProgressBar(int current, int total, const std::string& label = "Progress") {
        int barWidth = 40;
        float progress = (float)current / total;
        int pos = barWidth * progress;
        
        setColor(11); // Bright Cyan
        std::cout << "\r" << label << " [";
        
        for(int i = 0; i < barWidth; ++i) {
            if(i < pos) {
                setColor(10); // Bright Green
                std::cout << "█";
            } else if(i == pos) {
                setColor(14); // Bright Yellow
                std::cout << "▓";
            } else {
                setColor(8); // Dark Gray  
                std::cout << "░";
            }
        }
        
        setColor(11); // Bright Cyan
        std::cout << "] " << int(progress * 100.0) << "% ";
        setColor(15); // Bright White
        std::cout << "(" << current << "/" << total << ")";
        resetColor();
        std::cout.flush();
    }
    
    // Enhanced mathematical operations
    long long power(long long base, int exp) {
        if(exp == 0) return 1;
        if(exp == 1) return base;
        
        long long result = 1;
        for(int i = 0; i < exp; i++) {
            result *= base;
            if(result > 1e18) return -1; // Overflow protection
        }
        return result;
    }
    
    long long evaluateWord(const std::string& word) {
        if(word.empty()) return 0;
        
        long long value = 0;
        long long multiplier = 1;
        
        for(int i = word.length() - 1; i >= 0; i--) {
            char c = word[i];
            if(!isalpha(c)) {
                lastError = "Invalid character in word: " + std::string(1, c);
                return -1;
            }
            
            if(assignment.find(c) == assignment.end()) {
                lastError = "Letter not assigned: " + std::string(1, c);
                return -1;
            }
            
            value += assignment[c] * multiplier;
            multiplier *= 10;
            
            if(value > 1e15) { // Overflow protection
                lastError = "Number too large during evaluation";
                return -1;
            }
        }
        return value;
    }
    
    long long evaluateExpression(const std::vector<Term>& terms) {
        long long result = 0;
        
        for(const auto& term : terms) {
            long long wordValue = evaluateWord(term.word);
            if(wordValue == -1) return -1;
            
            // Apply exponent
            long long powValue = power(wordValue, term.exponent);
            if(powValue == -1) {
                lastError = "Exponent calculation overflow";
                return -1;
            }
            
            long long termValue = powValue * term.coefficient;
            result += termValue;
            
            if(result > 1e15 || result < -1e15) {
                lastError = "Expression evaluation overflow";
                return -1;
            }
        }
        return result;
    }
    
    bool isValidAssignment() {
        lastError = "";
        
        // Check leading zeros
        for(char c : leadingLetters) {
            if(assignment.find(c) != assignment.end() && assignment[c] == 0) {
                lastError = "Leading zero detected for letter: " + std::string(1, c);
                return false;
            }
        }
        
        long long leftSum = evaluateExpression(leftTerms);
        if(leftSum == -1) return false;
        
        long long rightSum = evaluateExpression(rightTerms);
        if(rightSum == -1) return false;
        
        return leftSum == rightSum;
    }
    
    bool solve(int index) {
        if(index == letterOrder.size()) {
            return isValidAssignment();
        }
        
        char letter = letterOrder[index];
        for(int digit = 0; digit <= 9; digit++) {
            if(usedDigits[digit]) continue;
            if(digit == 0 && leadingLetters.count(letter)) continue;
            
            assignment[letter] = digit;
            usedDigits[digit] = true;
            
            if(solve(index + 1)) {
                if(!showAllSolutions) return true;
                
                // Display current solution
                solutionCount++;
                displayCurrentSolution();
                
                if(solutionCount >= 10) { // Limit solutions displayed
                    printWarningBox("Showing first 10 solutions only...");
                    usedDigits[digit] = false;
                    assignment.erase(letter);
                    return true;
                }
            }
            
            usedDigits[digit] = false;
            assignment.erase(letter);
        }
        return false;
    }
    
    void displayCurrentSolution() {
        setColor(13); // Bright Magenta
        std::cout << "\n🎯 Solution #" << solutionCount << ":\n";
        resetColor();
        
        setColor(15); // Bright White
        for(const auto& p : assignment) {
            setColor(11); // Bright Cyan
            std::cout << p.first;
            setColor(15); // Bright White  
            std::cout << "=";
            setColor(10); // Bright Green
            std::cout << p.second << " ";
        }
        std::cout << "\n";
        resetColor();
    }
    
    void extractLetters() {
        letters.clear();
        leadingLetters.clear();
        
        auto processTerms = [&](const std::vector<Term>& terms) {
            for(const auto& term : terms) {
                for(char c : term.word) {
                    if(isalpha(c)) {
                        letters.insert(toupper(c));
                    }
                }
                if(!term.word.empty() && isalpha(term.word[0])) {
                    leadingLetters.insert(toupper(term.word[0]));
                }
            }
        };
        
        processTerms(leftTerms);
        processTerms(rightTerms);
        
        letterOrder.assign(letters.begin(), letters.end());
        std::sort(letterOrder.begin(), letterOrder.end());
    }
    
    std::string formatNumber(long long num) {
        if(num == 0) return "0";
        
        bool negative = num < 0;
        if(negative) num = -num;
        
        std::string str = std::to_string(num);
        std::string formatted = "";
        int count = 0;
        
        for(int i = str.length() - 1; i >= 0; i--) {
            if(count > 0 && count % 3 == 0) {
                formatted = "," + formatted;
            }
            formatted = str[i] + formatted;
            count++;
        }
        
        return negative ? "-" + formatted : formatted;
    }
    
    bool validateInput(const std::string& equation) {
        // Check for equals sign
        size_t equalCount = std::count(equation.begin(), equation.end(), '=');
        if(equalCount != 1) {
            lastError = "Equation must contain exactly one '=' sign";
            return false;
        }
        
        // Check for valid characters
        std::regex validChars("^[A-Za-z0-9+\\-*^=\\s()]+$");
        if(!std::regex_match(equation, validChars)) {
            lastError = "Equation contains invalid characters";
            return false;
        }
        
        // Check for empty sides
        size_t equalPos = equation.find('=');
        std::string leftSide = equation.substr(0, equalPos);
        std::string rightSide = equation.substr(equalPos + 1);
        
        if(leftSide.empty() || rightSide.empty()) {
            lastError = "Both sides of equation must be non-empty";
            return false;
        }
        
        return true;
    }
    
public:
    CryptarithmSolver() : usedDigits(10, false), solutionCount(0), showAllSolutions(false) {}
    
    void parseEquation(const std::string& equation) {
        if(!validateInput(equation)) {
            throw std::invalid_argument(lastError);
        }
        
        leftTerms.clear();
        rightTerms.clear();
        
        size_t equalPos = equation.find('=');
        std::string leftSide = equation.substr(0, equalPos);
        std::string rightSide = equation.substr(equalPos + 1);
        
        parseTerms(leftSide, leftTerms);
        parseTerms(rightSide, rightTerms);
        
        extractLetters();
        
        if(letters.size() > 10) {
            throw std::invalid_argument("Too many unique letters (maximum 10 allowed)");
        }
        
        if(letters.empty()) {
            throw std::invalid_argument("No letters found in equation");
        }
    }
    
    void parseTerms(const std::string& expression, std::vector<Term>& terms) {
        std::string current = "";
        int sign = 1;
        
        for(size_t i = 0; i <= expression.length(); i++) {
            char c = (i < expression.length()) ? expression[i] : '+';
            
            if(c == ' ') continue;
            
            if(c == '+' || c == '-' || i == expression.length()) {
                if(!current.empty()) {
                    parseSingleTerm(current, terms, sign);
                    current = "";
                }
                sign = (c == '-') ? -1 : 1;
            } else {
                current += c;
            }
        }
    }
    
    void parseSingleTerm(const std::string& term, std::vector<Term>& terms, int sign) {
        // Handle coefficient and exponent
        std::string cleanTerm = term;
        int coefficient = sign;
        int exponent = 1;
        
        // Extract coefficient
        size_t letterStart = 0;
        while(letterStart < cleanTerm.length() && 
              (isdigit(cleanTerm[letterStart]) || cleanTerm[letterStart] == '*')) {
            letterStart++;
        }
        
        if(letterStart > 0) {
            std::string coeffStr = cleanTerm.substr(0, letterStart);
            if(coeffStr.back() == '*') coeffStr.pop_back();
            
            if(!coeffStr.empty() && coeffStr != "*") {
                coefficient = sign * std::stoi(coeffStr);
            }
            cleanTerm = cleanTerm.substr(letterStart);
        }
        
        // Extract exponent (if exists)
        size_t expPos = cleanTerm.find('^');
        if(expPos != std::string::npos) {
            std::string expStr = cleanTerm.substr(expPos + 1);
            if(!expStr.empty() && isdigit(expStr[0])) {
                exponent = std::stoi(expStr);
            }
            cleanTerm = cleanTerm.substr(0, expPos);
        }
        
        if(!cleanTerm.empty()) {
            // Convert to uppercase for consistency
            std::transform(cleanTerm.begin(), cleanTerm.end(), cleanTerm.begin(), ::toupper);
            terms.emplace_back(cleanTerm, coefficient, exponent);
        }
    }
    
    bool solvePuzzle() {
        if(letters.size() > 10) {
            lastError = "Too many unique letters (maximum 10)";
            return false;
        }
        
        assignment.clear();
        usedDigits.assign(10, false);
        solutionCount = 0;
        lastError = "";
        
        return solve(0);
    }
    
    void displayProblem() {
        std::string problem = formatEquation();
        printGradientBox("CRYPTARITHM PROBLEM", problem, 13, 11);
    }
    
    std::string formatEquation() {
        std::string equation = "";
        
        // Left side
        for(size_t i = 0; i < leftTerms.size(); i++) {
            if(i > 0) {
                equation += (leftTerms[i].coefficient >= 0) ? " + " : " - ";
            } else if(leftTerms[i].coefficient < 0) {
                equation += "-";
            }
            
            int absCoeff = abs(leftTerms[i].coefficient);
            if(absCoeff != 1) {
                equation += std::to_string(absCoeff) + "*";
            }
            
            equation += leftTerms[i].word;
            
            if(leftTerms[i].exponent != 1) {
                equation += "^" + std::to_string(leftTerms[i].exponent);
            }
        }
        
        equation += " = ";
        
        // Right side
        for(size_t i = 0; i < rightTerms.size(); i++) {
            if(i > 0) {
                equation += (rightTerms[i].coefficient >= 0) ? " + " : " - ";
            } else if(rightTerms[i].coefficient < 0) {
                equation += "-";
            }
            
            int absCoeff = abs(rightTerms[i].coefficient);
            if(absCoeff != 1) {
                equation += std::to_string(absCoeff) + "*";
            }
            
            equation += rightTerms[i].word;
            
            if(rightTerms[i].exponent != 1) {
                equation += "^" + std::to_string(rightTerms[i].exponent);
            }
        }
        
        return equation;
    }
    
    void displaySolution() {
        if(assignment.empty()) {
            printErrorBox("No solution found for this cryptarithm!");
            if(!lastError.empty()) {
                setColor(12); // Bright Red
                std::cout << "Details: " << lastError << "\n";
                resetColor();
            }
            return;
        }
        
        printSuccessBox("Solution found successfully!");
        
        // Display letter assignments with colors
        std::string assignments = "";
        std::vector<std::pair<char, int>> sortedAssign(assignment.begin(), assignment.end());
        std::sort(sortedAssign.begin(), sortedAssign.end());
        
        for(const auto& p : sortedAssign) {
            assignments += std::string(1, p.first) + " = " + std::to_string(p.second) + "    ";
        }
        printGradientBox("LETTER ASSIGNMENTS", assignments, 10, 14);
        
        // Display verification with enhanced formatting
        displayVerification();
    }
    
    void displayVerification() {
        std::string verification = "";
        
        // Left side evaluation
        long long leftTotal = evaluateExpression(leftTerms);
        std::string leftEval = "Left Side:  ";
        
        for(size_t i = 0; i < leftTerms.size(); i++) {
            if(i > 0) {
                leftEval += (leftTerms[i].coefficient >= 0) ? " + " : " - ";
            } else if(leftTerms[i].coefficient < 0) {
                leftEval += "-";
            }
            
            long long wordValue = evaluateWord(leftTerms[i].word);
            long long termValue = power(wordValue, leftTerms[i].exponent) * abs(leftTerms[i].coefficient);
            leftEval += formatNumber(termValue);
        }
        leftEval += " = " + formatNumber(leftTotal);
        
        // Right side evaluation  
        long long rightTotal = evaluateExpression(rightTerms);
        std::string rightEval = "Right Side: ";
        
        for(size_t i = 0; i < rightTerms.size(); i++) {
            if(i > 0) {
                rightEval += (rightTerms[i].coefficient >= 0) ? " + " : " - ";
            } else if(rightTerms[i].coefficient < 0) {
                rightEval += "-";
            }
            
            long long wordValue = evaluateWord(rightTerms[i].word);
            long long termValue = power(wordValue, rightTerms[i].exponent) * abs(rightTerms[i].coefficient);
            rightEval += formatNumber(termValue);
        }
        rightEval += " = " + formatNumber(rightTotal);
        
        verification = leftEval + "\n" + rightEval + "\n";
        verification += "✓ Verification: " + formatNumber(leftTotal) + " = " + formatNumber(rightTotal);
        
        printGradientBox("SOLUTION VERIFICATION", verification, 10, 2);
    }
    
    void displayStatistics() {
        std::string stats = "";
        stats += "🔤 Unique Letters: " + std::to_string(letters.size()) + "\n";
        stats += "🚫 Leading Letters: " + std::to_string(leadingLetters.size()) + "\n";
        stats += "➕ Total Terms: " + std::to_string(leftTerms.size() + rightTerms.size()) + "\n";
        stats += "🧮 Complexity Level: ";
        
        if(letters.size() <= 4) stats += "Easy";
        else if(letters.size() <= 7) stats += "Medium";
        else stats += "Hard";
        
        printGradientBox("PUZZLE STATISTICS", stats, 9, 6);
    }
    
    void displayExamples() {
        setColor(14); // Bright Yellow
        std::cout << "\n📌 ";
        setColor(15); // Bright White
        std::cout << "Example equations you can try:\n";
        resetColor();
        
        printStyledSeparator('=', 11);
        
        struct Example {
            std::string equation;
            std::string description;
            int difficulty;
        };
        
        std::vector<Example> examples = {
            {"SEND + MORE = MONEY", "Classic cryptarithm puzzle", 1},
            {"TWO + TWO = FOUR", "Simple addition puzzle", 1},
            {"CROSS + ROADS = DANGER", "Medium complexity puzzle", 2},
            {"FORTY + TEN + TEN = SIXTY", "Multiple addends", 2},
            {"2*BASE + BALL = GAMES", "With coefficients", 3},
            {"SATURN + URANUS + NEPTUNE + PLUTO = PLANETS", "High complexity", 3},
            {"ABC^2 + DEF = GHIJ", "With exponents", 3},
            {"3*CAT + DOG = PETS", "Mixed operations", 2}
        };
        
        for(const auto& ex : examples) {
            setColor(11); // Bright Cyan
            std::cout << "• ";
            setColor(15); // Bright White
            std::cout << std::left << std::setw(35) << ex.equation;
            setColor(8); // Dark Gray
            std::cout << " │ ";
            setColor(14); // Bright Yellow
            std::cout << ex.description;
            setColor(8); // Dark Gray
            std::cout << " │ ";
            
            // Difficulty indicator
            for(int i = 0; i < ex.difficulty; i++) {
                setColor(12); // Bright Red
                std::cout << "★";
            }
            for(int i = ex.difficulty; i < 3; i++) {
                setColor(8); // Dark Gray
                std::cout << "☆";
            }
            std::cout << "\n";
        }
        
        printStyledSeparator('=', 11);
        resetColor();
    }
    
    void run() {
        printAnimatedHeader();
        displayExamples();
        
        while(true) {
            setColor(11); // Bright Cyan
            std::cout << "\n🔧 ";
            setColor(15); // Bright White
            std::cout << "Enter cryptarithm equation ";
            setColor(8); // Dark Gray
            std::cout << "(or 'help', 'examples', 'quit'): ";
            resetColor();
            
            std::string input;
            std::getline(std::cin, input);
            
            // Handle commands
            if(input == "quit" || input == "exit") {
                setColor(13); // Bright Magenta
                std::cout << "\n👋 ";
                setColor(15); // Bright White
                std::cout << "Thank you for using Enhanced Cryptarithm Solver!\n";
                resetColor();
                break;
            }
            
            if(input == "help") {
                displayHelp();
                continue;
            }
            
            if(input == "examples") {
                displayExamples();
                continue;
            }
            
            if(input.empty()) continue;
            
            try {
                std::cout << "\n";
                printStyledSeparator('=', 9);
                
                parseEquation(input);
                displayProblem();
                displayStatistics();
                
                setColor(14); // Bright Yellow
                std::cout << "\n🔍 ";
                setColor(15); // Bright White
                std::cout << "Solving puzzle";
                resetColor();
                
                // Animated solving indication
                for(int i = 0; i < 3; i++) {
                    std::cout << ".";
                    std::cout.flush();
                    std::chrono::milliseconds dura(300);
                    std::this_thread::sleep_for(dura);
                }
                std::cout << "\n";
                
                auto start = std::chrono::high_resolution_clock::now();
                bool solved = solvePuzzle();
                auto end = std::chrono::high_resolution_clock::now();
                
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                
                std::cout << "\n";
                displaySolution();
                
                // Performance info
                setColor(8); // Dark Gray
                std::cout << "\n⏱️  ";
                setColor(15); // Bright White
                std::cout << "Solving time: ";
                setColor(10); // Bright Green
                std::cout << duration.count() << " ms";
                
                if(solutionCount > 1) {
                    setColor(8); // Dark Gray
                    std::cout << " │ ";
                    setColor(13); // Bright Magenta
                    std::cout << "Multiple solutions found: " << solutionCount;
                }
                std::cout << "\n";
                resetColor();
                
                printStyledSeparator('=', 9);
                
            } catch(const std::exception& e) {
                printErrorBox(e.what());
            }
        }
    }
    
    void displayHelp() {
        printGradientBox("HELP - HOW TO USE", 
            "• Enter equations like: SEND + MORE = MONEY\n"
            "• Supported operations: +, -, * (coefficients)\n"
            "• Exponents: ABC^2 (square ABC)\n"
            "• Coefficients: 2*ABC (multiply ABC by 2)\n"
            "• Each letter represents a unique digit (0-9)\n"
            "• Leading letters cannot be zero\n"
            "• Commands: 'help', 'examples', 'quit'", 
            14, 11);
    }
    
    void setShowAllSolutions(bool show) {
        showAllSolutions = show;
    }
    
    int getSolutionCount() const {
        return solutionCount;
    }
    
    std::string getLastError() const {
        return lastError;
    }
    
    void clearSolution() {
        assignment.clear();
        solutionCount = 0;
        lastError = "";
    }
};

int main() {
    try {
        CryptarithmSolver solver;
        solver.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
            