#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <string>
#include <utility>
#include <algorithm>

using namespace std;

// Hash function for pair<int, string> so it can be used as a key in unordered_map
struct pair_hash {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

// Type aliases for readability
using RotationKey = pair<int, string>; // (round, rotation type)
using StudentList = vector<string>;
using RotationMap = unordered_map<RotationKey, StudentList, pair_hash>; // Use pair_hash here

// Function to parse the input file and populate the rotation map
RotationMap parseInput(const string& filename) {
    ifstream file(filename);
    RotationMap rotationMap;
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return rotationMap;
    }

    string line;
    vector<string> rotations = { "MED", "OBGY", "PEDS", "PSYC", "SURG", "FMED" };
    int lineIndex = 0;

    // Read each line and map it to the appropriate round and rotation type
    while (getline(file, line) && lineIndex < 36) {
        int round = (lineIndex / 6) + 1;                // Calculate round (1 to 6)
        string rotationType = rotations[lineIndex % 6]; // Determine rotation type based on line index

        RotationKey key(round, rotationType);
        istringstream ss(line);
        string name;

        // Parse comma-separated names on the line
        while (getline(ss, name, ',')) {
            // Trim whitespace from the name
            name = name.substr(name.find_first_not_of(" "), name.find_last_not_of(" ") + 1);
            rotationMap[key].push_back(name);
        }

        lineIndex++;
    }

    file.close();

    // Debug: Print loaded rotation data
    cout << "Parsed rotation data:\n";
    for (const auto& entry : rotationMap) {
        cout << "Round " << entry.first.first << " - " << entry.first.second << ": ";
        for (const auto& student : entry.second) {
            cout << student << ", ";
        }
        cout << "\n";
    }

    return rotationMap;
}


// Function to find direct and single-step indirect swaps
void findSwaps(const RotationMap& rotationMap, const string& targetStudent) {
    RotationKey target1(2, "SURG");
    RotationKey target2(5, "PEDS");

    bool foundDirect = false;
    bool foundIndirect = false;

    // Check for direct swaps
    cout << "\nChecking for direct swaps...\n";
    for (const auto& student : rotationMap.at(target1)) {
        // Ensure the student is not the target student to avoid swapping with oneself
        if (student != targetStudent && find(rotationMap.at(target2).begin(), rotationMap.at(target2).end(), student) != rotationMap.at(target2).end()) {
            cout << "Direct swap found with student: " << student << endl;
            foundDirect = true;
        }
    }

    if (!foundDirect) {
        cout << "No direct swaps found.\n";
    }

    // Check for single-step indirect swaps
    cout << "\nChecking for single-step indirect swaps...\n";
    for (const auto& studentInSURG : rotationMap.at(target1)) {
        if (studentInSURG == targetStudent) continue; // Skip if the student is the target student

        for (const auto& entry : rotationMap) {
            const auto& roundRotation = entry.first;
            const auto& students = entry.second;

            if (roundRotation.first == 5 && roundRotation.second != "PEDS") {
                for (const auto& studentInRound5 : students) {
                    if (studentInRound5 == targetStudent || studentInRound5 == studentInSURG) continue; // Skip if the student would swap with themselves

                    // Look for students in PEDS on round 5 who can swap indirectly
                    for (const auto& studentInPEDS : rotationMap.at(target2)) {
                        if (studentInPEDS != targetStudent && find(rotationMap.at({ 2, roundRotation.second }).begin(), rotationMap.at({ 2, roundRotation.second }).end(), studentInPEDS) != rotationMap.at({ 2, roundRotation.second }).end()) {
                            cout << "Indirect swap sequence found:\n";
                            cout << "1. Swap " << studentInSURG << " in SURG round 2 with " << studentInRound5 << " in " << roundRotation.second << " round 5.\n";
                            cout << "2. Then swap " << studentInRound5 << " in PEDS round 5 with " << studentInPEDS << " in " << roundRotation.second << " round 2.\n";
                            foundIndirect = true;
                        }
                    }
                }
            }
        }
    }

    if (!foundIndirect) {
        cout << "No single-step indirect swaps found." << endl;
    }
}

int main() {
    string targetStudent = "Phoebe Sotiroff";
    RotationMap rotationMap = parseInput("C:\\Users\\zebkl\\code\\clinical_rotations\\input.txt");

    // Check if target student is in the rotation map
    bool found = false;
    for (const auto& entry : rotationMap) {
        const auto& students = entry.second;

        if (find(students.begin(), students.end(), targetStudent) != students.end()) {
            found = true;
            break;
        }
    }

    if (!found) {
        cerr << "Error: Target student " << targetStudent << " not found in the input data." << endl;
        return 1;
    }

    findSwaps(rotationMap, targetStudent);

    return 0;
}
