#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

// Struct to store each team's data (name, location, stadium)
struct Team {
    std::string name;
    std::string location;
    std::string stadium;
};

// Function to load teams from the teams.csv file into the 'teams' vector
std::vector<Team> loadTeamsFromCSV(const std::string& filename) {
    std::vector<Team> teams;
    std::ifstream file(filename);  // Open the file
    
    std::cout << "Attempting to open file: " << filename << std::endl; //Output to terminal

    if (!file.is_open()) {
        // Error handling if the file can't be opened
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return teams; // Return empty vector if the file can't be opened
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);  // Read each line into a stringstream
        Team team;

        // Read and store the team details (name, location, stadium) from each line
        std::getline(ss, team.name, ',');
        std::getline(ss, team.location, ',');
        std::getline(ss, team.stadium, ',');

        teams.push_back(team);  // Add the team to the vector
    }

    file.close();  // Close the file after reading all teams
    return teams;
}

// Function to write generated fixtures to a CSV file (fixtures.csv)
void writeFixturesToCSV(const std::vector<std::string>& fixtures, const std::string& filename) {
    std::ofstream file(filename);  // Open the file for writing
    
    if (!file.is_open()) {
        // Error handling if the file can't be opened
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Write the CSV header for the matches
    file << "Match,Leg,Location,Stadium\n";

    // Write each fixture line into the file
    for (const auto& line : fixtures) {
        file << line << std::endl;
    }

    file.close();  // Close the file once all fixtures are written
    std::cout << "Fixtures written to " << filename << std::endl;
}

// Function to generate the match fixtures
void generateFixtures(const std::vector<Team>& teams) {
    std::unordered_map<std::string, std::vector<Team>> townTeams;

    // Group the teams based on their locations (towns)
    for (const auto& team : teams) {
        townTeams[team.location].push_back(team);
    }

    // This will store the matchups as pairs of teams
    std::vector<std::pair<Team, Team>> fixtures;
    std::unordered_set<std::string> playedPairs;  // Keeps track of played matches
    std::vector<std::string> outputLines;  // Stores the output lines for writing to CSV

    // Create matchups between teams from different towns
    for (const auto& town : townTeams) {
        const auto& townTeamList = town.second;

        for (const auto& otherTown : townTeams) {
            if (town.first == otherTown.first) continue;  // Skip matches within the same town

            for (const auto& homeTeam : townTeamList) {
                for (const auto& awayTeam : otherTown.second) {
                    std::string pairKey = homeTeam.name + " vs " + awayTeam.name;

                    // Check if the match has been played before (avoid duplicates)
                    if (playedPairs.find(pairKey) == playedPairs.end()) {
                        fixtures.emplace_back(homeTeam, awayTeam);  // Add match to the fixtures
                        playedPairs.insert(pairKey);  // Mark the match as played
                    }
                }
            }
        }
    }

    // Variable to track the number of weekends (1 weekend = 4 matches)
    int totalWeeks = 0;

    // These vectors will store the match details for Leg 1 and Leg 2
    std::vector<std::string> leg1Lines, leg2Lines;

    // Generating Leg 1 fixtures
    for (size_t i = 0; i < fixtures.size(); ++i) {
        // Start a new weekend after every 4 matches
        if (i % 4 == 0) {
            totalWeeks++;  // Increment the weekend counter
            if (totalWeeks > 40) {  // We limit to 40 weekends max
                std::cout << "Reached maximum number of weekends." << std::endl;
                break;
            }
            // Add the weekend header
            leg1Lines.push_back("Weekend " + std::to_string(totalWeeks) + ":");
        }

        // Generate match details for Leg 1
        const auto& match = fixtures[i];
        std::string leg1Line = match.first.name + " (Home) vs " + match.second.name + " (Away),Leg 1," +
                               match.first.location + "," + match.first.stadium;
        leg1Lines.push_back(leg1Line);  // Add match details for the weekend
    }

    // Generating Leg 2 fixtures (reverse of Leg 1)
    for (size_t i = 0; i < fixtures.size(); ++i) {
        // Start a new weekend after every 4 matches for Leg 2
        if (i % 4 == 0) {
            totalWeeks++;  // Increment the weekend counter
            if (totalWeeks > 40) {  // Again, limit to 40 weekends
                std::cout << "Reached maximum number of weekends." << std::endl;
                break;
            }
            // Add the weekend header for Leg 2
            leg2Lines.push_back("Weekend " + std::to_string(totalWeeks) + ":");
        }

        // Generate match details for Leg 2 (reverse of Leg 1)
        const auto& match = fixtures[i];
        std::string leg2Line = match.second.name + " (Home) vs " + match.first.name + " (Away),Leg 2," +
                               match.second.location + "," + match.second.stadium;
        leg2Lines.push_back(leg2Line);  // Add match details for the weekend
    }

    // Combine both legs into the final output
    outputLines.insert(outputLines.end(), leg1Lines.begin(), leg1Lines.end());
    outputLines.insert(outputLines.end(), leg2Lines.begin(), leg2Lines.end());

    // Write the generated fixtures to the CSV file
    std::string output_filename = "C:\\Users\\elija\\OneDrive\\Documents\\MatchFixtureGenerator\\fixtures.csv";
    writeFixturesToCSV(outputLines, output_filename);  // Save the output to the CSV
}

int main() {
    // File path to the CSV with team data
    std::string input_filename = "C:\\Users\\elija\\OneDrive\\Documents\\MatchFixtureGenerator\\teams.csv";
    
    // Load teams from the CSV file
    std::vector<Team> teams = loadTeamsFromCSV(input_filename);

    if (teams.empty()) {
        // If no teams are loaded, print an error message
        std::cout << "No teams loaded. Please check the teams.csv file." << std::endl;
    } else {
        // If teams are loaded successfully, print their details
        std::cout << "Teams loaded successfully:" << std::endl;
        for (const auto& team : teams) {
            std::cout << "Name: " << team.name << ", Location: " << team.location << ", Stadium: " << team.stadium << std::endl;
        }
        // Call the function to generate fixtures
        generateFixtures(teams);
    }

    return 0;
}
