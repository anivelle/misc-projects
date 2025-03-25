"""
    Scrapes Wikipedia pages of constellations to collate all stars (well, those
    in the Hipparcos catalogue) for use in the celestial globe rendering.
    Technically I could use this to also gather the RA and DEC instead of using
    the Hipparcos catalogue but I did all that before and the Wikipedia entries
    have Unicode characters that I don't wanna deal with
    
"""
import pandas as pd
from bs4 import BeautifulSoup 
import requests
import io


def main(): 
    r = requests.get("https://en.wikipedia.org/wiki/Lists_of_stars_by_constellation")
    # constellations = pd.read_html(io.StringIO(r.text))
    soup = BeautifulSoup(r.text, "html.parser")
    table = soup.find("table")
    with(open("constellations.csv", "w") as touch):
        pass

    for constellation in table.find_all("a"):
        # if (constellation.string in ["Orion"]):
        #     continue
        url_name = constellation.string.replace(" ", "_")

        print(url_name)
        r = requests.get(f"https://en.wikipedia.org/wiki/List_of_stars_in_{url_name}")
        data = pd.read_html(io.StringIO(r.text), header=0)
        # print(data[1])
        index = 0
        if (url_name == "Vulpecula"):
            index = 1
        data = data[index][["HIP"]].apply(lambda x: pd.to_numeric(x, downcast="integer", errors='coerce'))
        data.dropna(subset=["HIP"], inplace=True)

        # print(data["HIP"])
        data["HIP"].to_csv("constellations.csv", mode="a", header=False, index=False)

if __name__ == "__main__":
    main()
