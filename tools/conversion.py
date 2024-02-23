import os
# This script reads all *.sms files in the current directory and copies them to the 'converted' directory
# with a 32-bit header containing the size of the file.
# Create 'converted' directory if it doesn't exist
if not os.path.exists("converted"):
    os.makedirs("converted")

# Get all *.sms files in the current directory
sms_files = [file for file in os.listdir() if file.endswith('.sms')]

for file_name in sms_files:
    # Read the contents of the file
    with open(file_name, 'rb') as source_file:
        file_contents = source_file.read()

    # Get the size of the file
    file_size = len(file_contents)

    # Create destination filename in the 'converted' directory
    destination_filename = os.path.join("converted", file_name)

    # Write the file size as a 32-bit header to the destination file
    with open(destination_filename, 'wb') as destination_file:
        header = file_size.to_bytes(4, byteorder='little')
        destination_file.write(header)

        # Append the contents of the source file to the destination file
        destination_file.write(file_contents)

    print(f"File copied successfully: {file_name}")

print("All files copied successfully.")

