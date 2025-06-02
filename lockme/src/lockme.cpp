/* --- [ WHAT IS THIS ] ---
 * Lockme is a simple program to lock this computer remotely from a phone on the same network.
 * A counterpart android app should be used to lock the computer.
 * 
 * --- [ STRUCTURE ] ---
 * - define variables like port and lockword
 * - init udp/datagram socket
 * - listen for incoming requests. If the request matches the request lockword, lock the computer.
 * 
 * --- [ USAGE ] ---
 * computer: add program to start on boot to run in background. Possibly configure a file with the lockword and port.
 * phone: install app. Configure lockword and port in a settings page. Press the big button on the main page to broadcast the lock request.
*/

#include <iostream>

int main()
{
    std::cout << "Hello World!\n";
}

