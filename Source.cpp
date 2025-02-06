#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <cctype>

// Function to convert a string to lowercase for case-insensitive comparison
std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

// Function to check if a URL is blocked (case-insensitive comparison)
bool isUrlBlocked(const std::unordered_set<std::string>& blockedUrls, const std::string& url) {
    std::string lowerUrl = toLowerCase(url);
    return blockedUrls.find(lowerUrl) != blockedUrls.end();
}

// Function to load blocked URLs from a file
std::unordered_set<std::string> loadBlockedUrlsFromFile(const std::string& filename) {
    std::unordered_set<std::string> blockedUrls;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open the file containing blocked URLs." << std::endl;
        return blockedUrls;  // Return empty set if file can't be opened
    }
    std::string url;
    while (std::getline(file, url)) {
        blockedUrls.insert(toLowerCase(url));  // Store URLs in lowercase for case-insensitive comparison
    }
    return blockedUrls;
}

int main() {
    // --- Happy Path Tests ---
    std::cout << "--- Happy Path Tests ---" << std::endl;

    // Happy Path Test Case 1: Basic blocking
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com", "http://test.com" };
        assert(isUrlBlocked(blockedUrls, "http://example.com") == true);
        assert(isUrlBlocked(blockedUrls, "http://notblocked.com") == false);
        std::cout << "Happy Path Test Case 1 passed!" << std::endl;
    }

    // Happy Path Test Case 2: HTTPS URLs
    {
        std::unordered_set<std::string> blockedUrls = { "https://site.com", "https://another.com" };
        assert(isUrlBlocked(blockedUrls, "https://site.com") == true);
        assert(isUrlBlocked(blockedUrls, "https://notblocked.com") == false);
        std::cout << "Happy Path Test Case 2 passed!" << std::endl;
    }

    // --- Edge Case Tests ---
    std::cout << "\n--- Edge Case Tests ---" << std::endl;

    // Edge Case Test Case 1: Case Insensitivity
    {
        std::unordered_set<std::string> blockedUrls = { "HTTP://EXAMPLE.COM", "HTTP://TEST.COM" };
        assert(isUrlBlocked(blockedUrls, "http://example.com") == true);
        assert(isUrlBlocked(blockedUrls, "http://test.com") == true);
        assert(isUrlBlocked(blockedUrls, "Http://Example.Com") == true); // Mixed case
        std::cout << "Edge Case Test Case 1 passed!" << std::endl;
    }

    // Edge Case Test Case 2: Empty URL List
    {
        std::unordered_set<std::string> blockedUrls; // Empty set
        assert(isUrlBlocked(blockedUrls, "http://example.com") == false);
        std::cout << "Edge Case Test Case 2 passed!" << std::endl;
    }

    // Edge Case Test Case 3: Empty URL to Check
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com" };
        assert(isUrlBlocked(blockedUrls, "") == false);
        std::cout << "Edge Case Test Case 3 passed!" << std::endl;
    }

    // Edge Case Test Case 4: URL is a substring of a blocked URL
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com" };
        assert(isUrlBlocked(blockedUrls, "http://example.co") == false); // Not an exact match
        std::cout << "Edge Case Test Case 4 passed!" << std::endl;
    }

    // Edge Case Test Case 5: Blocked URL is a substring of URL to check
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.co" };
        assert(isUrlBlocked(blockedUrls, "http://example.com") == false); // Not an exact match
        std::cout << "Edge Case Test Case 5 passed!" << std::endl;
    }

    // Edge Case Test Case 6: URL with trailing slash
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com/" };
        assert(isUrlBlocked(blockedUrls, "http://example.com/") == true);
        assert(isUrlBlocked(blockedUrls, "http://example.com") == false); // Important: different URLs
        std::cout << "Edge Case Test Case 6 passed!" << std::endl;
    }

    // Edge Case Test Case 7: URL with different ports
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com:8080" };
        assert(isUrlBlocked(blockedUrls, "http://example.com:8080") == true);
        assert(isUrlBlocked(blockedUrls, "http://example.com") == false); // Different port
        std::cout << "Edge Case Test Case 7 passed!" << std::endl;
    }

    // Edge Case Test Case 8: URL with query parameters
    {
        std::unordered_set<std::string> blockedUrls = { "http://example.com?param1=value1" };
        assert(isUrlBlocked(blockedUrls, "http://example.com?param1=value1") == true);
        assert(isUrlBlocked(blockedUrls, "http://example.com") == false); // Different URL
        std::cout << "Edge Case Test Case 8 passed!" << std::endl;
    }

    // --- File Loading Tests ---
    std::cout << "\n--- File Loading Tests ---" << std::endl;

    // File Loading Test Case 1: Valid file load
    {
        const std::string filename = "blocked_urls.txt"; // Create a dummy file
        std::ofstream outfile(filename);
        outfile << "http://block1.com\nhttps://block2.com\nHTTP://BLOCK3.COM";
        outfile.close();

        std::unordered_set<std::string> blockedUrls = loadBlockedUrlsFromFile(filename);
        assert(blockedUrls.size() == 3);
        assert(isUrlBlocked(blockedUrls, "http://block1.com") == true);
        assert(isUrlBlocked(blockedUrls, "https://block2.com") == true);
        assert(isUrlBlocked(blockedUrls, "http://block3.com") == true); // Case-insensitive
        std::cout << "File Loading Test Case 1 passed!" << std::endl;

        std::remove(filename.c_str()); // Clean up the dummy file
    }

    // File Loading Test Case 2: File does not exist
    {
        const std::string filename = "nonexistent_file.txt";
        std::unordered_set<std::string> blockedUrls = loadBlockedUrlsFromFile(filename);
        assert(blockedUrls.empty() == true); // Should return an empty set
        std::cout << "File Loading Test Case 2 passed!" << std::endl;
    }

    // File Loading Test Case 3: Empty file
    {
        const std::string filename = "empty_file.txt";
        std::ofstream outfile(filename);
        outfile.close();

        std::unordered_set<std::string> blockedUrls = loadBlockedUrlsFromFile(filename);
        assert(blockedUrls.empty() == true);
        std::cout << "File Loading Test Case 3 passed!" << std::endl;

        std::remove(filename.c_str());
    }

    // File Loading Test Case 4: File with empty lines and whitespace
    {
        const std::string filename = "whitespace_file.txt";
        std::ofstream outfile(filename);
        outfile << "   \nhttp://block1.com\n  https://block2.com   \n\n";
        outfile.close();

        std::unordered_set<std::string> blockedUrls = loadBlockedUrlsFromFile(filename);
        assert(blockedUrls.size() == 3);  //Important: Depends on if you trim whitespace in the loading function.  Currently it does NOT trim.
        assert(isUrlBlocked(blockedUrls, "http://block1.com") == true); 
        
        std::cout << "File Loading Test Case 4 passed!" << std::endl;

        std::remove(filename.c_str());
    }

    return 0;
}
