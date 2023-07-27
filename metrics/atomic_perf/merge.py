from PIL import Image
import os

def merge_and_concatenate_images(image1_path, image2_path, output_path):
    image1 = Image.open(image1_path)
    image2 = Image.open(image2_path)

    # Get the dimensions of the images
    width1, height1 = image1.size
    width2, height2 = image2.size

    # Calculate the size of the combined image
    total_width = width1 + width2
    max_height = max(height1, height2)

    # Create a new blank image with the calculated dimensions
    combined_image = Image.new('RGB', (total_width, max_height))

    # Paste the first image on the left side of the combined image
    combined_image.paste(image1, (0, 0))

    # Paste the second image on the right side of the combined image
    combined_image.paste(image2, (width1, 0))

    # Save the merged and concatenated image
    combined_image.save(output_path)
    print(f"Images merged and concatenated successfully! Saved as {output_path}")

if __name__ == "__main__":
    try:
        ps=os.environ['PS']
    except:
        print("ENV NOT FOUND")
        exit(1)
    # Replace these paths with the actual paths to your PNG images
    input_image2_path = f"img/{ps}_cost.png"
    input_image1_path = f"img/{ps}_cost_bar.png"
    output_image_path = f"img/{ps}_merged.png"

    merge_and_concatenate_images(input_image1_path, input_image2_path, output_image_path)
