#!/bin/sh
# Installation and configuration script for the Weaver package (TGZ/ZIP).
# Must be run AFTER unpacking the tarball.

PROJECT_NAME="Weaver"

echo "=================================================="
echo "Configuring the ${PROJECT_NAME} application..."
echo "=================================================="

# 1. Determine the HOME directory of the current user (the one running the script)
# We use whoami and getent passwd to be robust.
USER_NAME=$(whoami)
USER_HOME=$(getent passwd $USER_NAME | cut -d: -f6)

# 2. Define the path to the user folder
PROJECT_DIR="${USER_HOME}/${PROJECT_NAME}"

# 3. Create the directories
echo "Creating configuration directories in: ${PROJECT_DIR}"
mkdir -p "$PROJECT_DIR/output"
mkdir -p "$PROJECT_DIR/user_config"

# 4. Assign ownership (mainly for security purposes, although the script is executed by the user themselves)
chown -R "$USER_NAME:$USER_NAME" "$PROJECT_DIR"

echo "Initial configuration completed."
echo "To uninstall, simply delete the folder where you extracted the binaries."

exit 0