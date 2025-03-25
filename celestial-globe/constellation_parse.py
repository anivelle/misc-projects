"""
    Scrapes Wikipedia pages of constellations to collate all stars (well, those
    in the Hipparcos catalogue) for use in the celestial globe rendering.
    Technically I could use this to also gather the RA and DEC instead of using
    the Hipparcos catalogue but I did all that before and the Wikipedia entries
    have Unicode characters that I don't wanna deal with
    
"""
import pandas as pd
import requests
import io


def main(): 
    r = requests.get("https://en.wikipedia.org/wiki/Lists_of_stars_by_constellation")
    constellations = pd.read_html(io.StringIO(r.text))
    
    for i in constellations[0].columns:
        print(constellations[0][i])
    # r = requests.get("https://en.wikipedia.org/wiki/List_of_stars_in_Ursa_Major")
    # print(r.text)
    # data = pd.read_html(r.text, header=0)
    # print(data[0]["HIP"])
    # data[0]["HIP"].to_csv("ursa_major.csv")

if __name__ == "__main__":
    main()
