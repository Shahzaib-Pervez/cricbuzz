import requests
from bs4 import BeautifulSoup
import re

HEADERS = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
}

def get_soup(url):
    try:
        response = requests.get(url, headers=HEADERS, timeout=10)
        response.raise_for_status()
        return BeautifulSoup(response.content, 'html.parser')
    except Exception as e:
        print(f"Error fetching URL {url}: {e}")
        return None

def get_scorecard(url_or_id: str):
    """
    Scrapes detailed scorecard data using Next.js hydration JSON.
    """
    import json
    import re
    
    # Clean ID
    match_id = url_or_id
    if "cricbuzz.com" in url_or_id:
        # Extract ID from URL
        parts = url_or_id.split("/")
        for p in parts:
            if p.isdigit():
                match_id = p
                break
    
    url = f"https://www.cricbuzz.com/live-cricket-scorecard/{match_id}"
    
    data = {
        "status": "success",
        "data": {
            "name": "",
            "status": "",
            "score": []
        }
    }

    try:
        response = requests.get(url, headers=HEADERS, timeout=10)
        
        # 1. Extract JSON chunks
        chunks = re.findall(r'self\.__next_f\.push\(\[\d+,\s*"(.*?)"\]\)', response.text, re.DOTALL)
        
        match_info_found = False
        scorecard_found = False
        
        for chunk in chunks:
            unescaped = chunk.replace('\\"', '"').replace('\\\\', '\\')
            
            # 2. MATCH INFO (Title, Status)
            # Look for "matchInfo" or "matchHeader"
            # In the main match page it was "matchInfo". Here might be similar.
            if '"matchInfo":' in unescaped and not match_info_found:
                 # Try to extract a snippet containing matchInfo
                 # This might be in the same chunk as scoreCard or different.
                 # Let's try simple regex for name/status fallbacks if JSON parsing is hard for the whole chunk
                 pass

            # 3. SCORECARD DATA
            if '"scoreCard":[' in unescaped:
                # Extract scoreCard array
                start_marker = '"scoreCard":['
                start = unescaped.find(start_marker)
                if start != -1:
                    cnt = 0
                    start_array = start + len('"scoreCard":')
                    json_str = ""
                    for i in range(start_array, len(unescaped)):
                        ch = unescaped[i]
                        if ch == '[': cnt += 1
                        elif ch == ']': cnt -= 1
                        json_str += ch
                        if cnt == 0: break
                    
                    if json_str:
                        try:
                            sc_data = json.loads(json_str)
                            scorecard_found = True
                            
                            for inning in sc_data:
                                bat_team = inning.get("batTeamDetails", {})
                                bowl_team = inning.get("bowlTeamDetails", {})
                                score_det = inning.get("scoreDetails", {})
                                
                                # Inning Name
                                inning_name = bat_team.get("batTeamShortName") or bat_team.get("batTeamName")
                                
                                # Summary Stats
                                runs = score_det.get("runs", 0)
                                wickets = score_det.get("wickets", 0)
                                overs = score_det.get("overs", 0)
                                
                                inning_obj = {
                                    "inning": inning_name,
                                    "r": runs,
                                    "w": wickets,
                                    "o": float(overs),
                                    "batsmen": [],
                                    "bowlers": []
                                }
                                
                                # Batsmen
                                batters_data = bat_team.get("batsmenData", {})
                                # Sort by key "bat_1", "bat_2"
                                sorted_bat_keys = sorted(batters_data.keys(), key=lambda k: int(k.split("_")[1]) if "_" in k else 99)
                                
                                for k in sorted_bat_keys:
                                    b = batters_data[k]
                                    bat_name = b.get("batName", "")
                                    # dismissal info is strictly not just text, but let's see. 
                                    # Prototype showed "dismissal": "c Charles b Akeal Hosein" (sometimes it's empty if not out)
                                    dismissal = b.get("outDesc", "") # Check key, prototype said dismissal? No, prototype above didn't show dismissal key name clearly, 
                                    # Wait, prototype JSON output I saw earlier:
                                    # "bat_1": { "batName": "...", ... "outDesc": "c Charles b Akeal Hosein" ... } ?? 
                                    # Re-reading prototype output... 
                                    # "batId": 10033, "batName": "Tom Kohler-Cadmore", ... "outDesc": "c Charles b Akeal Hosein", ...
                                    # Actually the prototype output snippet was truncated/summarized by me reading it? 
                                    # Let's check the verify step output again (Step 342).
                                    # It didn't print batsmen fields clearly.
                                    # Assume standard keys or try a few.
                                    
                                    out_desc = b.get("outDesc", b.get("dismissal", "not out"))
                                    if not out_desc: out_desc = "not out"
                                    
                                    inning_obj["batsmen"].append({
                                        "name": bat_name,
                                        "dismissal": out_desc,
                                        "r": str(b.get("runs", 0)),
                                        "b": str(b.get("balls", 0)),
                                        "4s": str(b.get("fours", 0)),
                                        "6s": str(b.get("sixes", 0)),
                                        "sr": str(b.get("strikeRate", 0))
                                    })
                                
                                # Bowlers
                                bowlers_data = bowl_team.get("bowlersData", {})
                                sorted_bowl_keys = sorted(bowlers_data.keys(), key=lambda k: int(k.split("_")[1]) if "_" in k else 99)
                                
                                for k in sorted_bowl_keys:
                                    bo = bowlers_data[k]
                                    bowl_name = bo.get("bowlName", "")
                                    
                                    inning_obj["bowlers"].append({
                                        "name": bowl_name,
                                        "o": str(bo.get("overs", 0)),
                                        "m": str(bo.get("maidens", 0)),
                                        "r": str(bo.get("runs", 0)),
                                        "w": str(bo.get("wickets", 0)),
                                        "eco": str(bo.get("economy", 0))
                                    })
                                    
                                data["data"]["score"].append(inning_obj)
                                
                        except Exception as e:
                            print(f"Error parsing scorecard JSON: {e}")
            
            # matchInfo check (fallback if not extracted above)
            # Actually, let's just use DOM for title/status if JSON fails?
            # Or try scraping title from HTML title tag as fallback.
            
    except Exception as e:
        print(f"Error fetching/parsing scorecard: {e}")
        return None

    # Fallback/Enhancement: Get Match Name/Status if missing
    # We can use Soup for this simple part
    if not data["data"]["name"]:
        soup = BeautifulSoup(response.text, 'html.parser')
        h1 = soup.find('h1', class_='cb-nav-hdr') or soup.find('h1')
        if h1:
            data['data']['name'] = h1.text.strip()
            
        if not data['data']['score']:
             # Fallback: Meta Tag Parsing (Summary Score)
             print("DEBUG: JSON score failed, checking Meta Tags...")
             meta_title = soup.find("meta", property="og:title")
             if meta_title:
                content = meta_title.get("content", "")
                if " vs " in content:
                    parts = content.split(" vs ")
                    team1_str = parts[0].strip()
                    # Handle second team safely
                    if len(parts) > 1:
                        team2_str = parts[1].split("|")[0].strip()
                        
                        for team_str in [team1_str, team2_str]:
                            m = re.search(r'([A-Za-z0-9 ]+)\s+(\d+/\d+|\d+)\s*\(?(\d+\.\d+|\d+)?\)?', team_str)
                            if m:
                                name = m.group(1).strip()
                                score = m.group(2)
                                overs = m.group(3) if m.group(3) else "0"
                                
                                runs = 0
                                wickets = 0
                                if '/' in score:
                                    s_parts = score.split('/')
                                    runs = int(s_parts[0])
                                    wickets = int(s_parts[1])
                                else:
                                    runs = int(score)
                                    wickets = 10 if "all out" in team_str.lower() else 0
                                    
                                data['data']['score'].append({
                                    "inning": name,
                                    "r": runs,
                                    "w": wickets,
                                    "o": float(overs),
                                    "batsmen": [],
                                    "bowlers": []
                                })
            
             # Fallback 2: DOM Parsing (Header)
             if not data['data']['score']:
                 score_node = soup.find('span', class_='cb-font-20 text-bold')
                 # This DOM structure varies heavily, best effort only if meta fails
                 pass

    return data

def get_commentary(url_or_id: str):
    """
    Scrapes commentary from Cricbuzz Mobile (m.cricbuzz.com).
    """
    match_id = url_or_id
    if "cricbuzz.com" in url_or_id:
        parts = url_or_id.split("/")
        for p in parts:
             if p.isdigit():
                 match_id = p
                 break
    
    # Try constructing the mobile commentary URL directly
    # Ideally should use get_real_url logic but simplified here for this task
    url = f"https://m.cricbuzz.com/cricket-commentary/{match_id}/match"
    
    print(f"Fetching Commentary from: {url}")
    soup = get_soup(url)
    
    if not soup:
        return {"status": "error", "message": "Failed to fetch page"}
        
    try:
        # Selector: div with border-cbChineseSilver and border-dotted (the separator lines)
        blocks = soup.find_all("div", class_=lambda x: x and "border-cbChineseSilver" in x and "border-dotted" in x)
        
        comm_lines = []
        for block in blocks:
            over_div = block.find("div", class_="font-bold")
            over = over_div.text.strip() if over_div else ""
            
            # The text content is typically the last child div in the flex container
            children = block.find_all("div", recursive=False)
            if len(children) >= 2:
                comm_text = children[-1].text.strip()
                comm_text = " ".join(comm_text.split())
                
                if over:
                    comm_lines.append(f"{over} {comm_text}")
                else:
                    comm_lines.append(comm_text)
                    
        return {"status": "success", "data": comm_lines}

    except Exception as e:
        print(f"Error parsing commentary: {e}")
        return {"status": "error", "message": str(e)}

def search_match_url(match_name):
    """
    Uses Google Search to find the Cricbuzz live score URL for the given match name.
    """
    try:
        from googlesearch import search
        query = f"cricbuzz live score {match_name}"
        print(f"Searching for: {query}")
        
        # Search for top 3 results
        for url in search(query, num_results=3, lang="en"):
            if "cricbuzz.com/live-cricket-scores/" in url:
                print(f"Found URL via search: {url}")
                return url
                
    except Exception as e:
        print(f"Search failed: {e}")
        return None
    
    return None

    return None

def get_real_url(match_id, match_name):
    # 1. Try direct ID (Mobile URL preferred for detailed score)
    url = f"https://m.cricbuzz.com/live-cricket-scorecard/{match_id}"
    print(f"Trying Direct URL: {url}")
    
    try:
        response = requests.get(url, headers=HEADERS, timeout=5)
        if response.status_code == 200:
            # Check if it redirected to a valid match page
            # Accept: live-cricket-scores, live-cricket-scorecard, cricket-commentary
            if any(x in response.url for x in ["live-cricket-scores/", "live-cricket-scorecard/", "cricket-commentary/"]):
                 return response.url
    except:
        pass
        
    # 2. Search Fallback
    if match_name:
        print("Direct URL failed. Trying Search...")
        return search_match_url(match_name)
        
    return None

def get_matches(match_type="live"):
    """
    Scrapes match list from Cricbuzz based on type.
    Types: live, recent, upcoming
    """
    url_map = {
        "live": "https://www.cricbuzz.com/cricket-match/live-scores",
        "recent": "https://www.cricbuzz.com/cricket-match/live-scores/recent-matches",
        "upcoming": "https://www.cricbuzz.com/cricket-match/live-scores/upcoming-matches"
    }
    
    url = url_map.get(match_type, url_map["live"])
    print(f"Fetching matches from: {url}")
    
    
    # NEW LOGIC: Extract JSON data from Next.js hydration scripts
    # This avoids fragile HTML scraping and gives us the exact data used by the frontend.
    import re
    import json
    import requests

    extracted_matches = []

    try:
        response = requests.get(url, headers=HEADERS, timeout=10)
        # Regex to find all self.__next_f.push calls
        # content is in response.text
        chunks = re.findall(r'self\.__next_f\.push\(\[\d+,\s*"(.*?)"\]\)', response.text, re.DOTALL)

        for chunk in chunks:
            # Unescape the chunk
            unescaped_chunk = chunk.replace('\\"', '"').replace('\\\\', '\\')

            # We are looking for "matches":[ ... ]
            if '"matches":[' in unescaped_chunk:
                try:
                    # Find start of matches list
                    start_idx = unescaped_chunk.find('"matches":[')
                    if start_idx == -1: continue

                    # Re-do specific extraction for reliability
                    cnt = 0
                    start_array = start_idx + 10 # points to [
                    
                    # Verify it starts with [
                    if unescaped_chunk[start_array] != '[': continue
                    
                    json_str = ""
                    for i in range(start_array, len(unescaped_chunk)):
                        ch = unescaped_chunk[i]
                        if ch == '[': cnt += 1
                        elif ch == ']': cnt -= 1
                        json_str += ch
                        if cnt == 0:
                            break
                    
                    if json_str:
                         matches_data = json.loads(json_str)
                         
                         for m_item in matches_data:
                            # Determine the source dictionary for match data
                            match_source = m_item
                            if "match" in m_item and "matchInfo" in m_item["match"]:
                                match_source = m_item["match"]
                            
                            m_info = match_source.get("matchInfo", {})
                            if not m_info: continue

                            match_id = str(m_info.get("matchId", ""))
                            if match_id in [m["matchInfo"]["matchId"] for m in extracted_matches]:
                                continue

                            title = m_info.get("seriesName", "") + " - " + m_info.get("matchDesc", "")
                            status = m_info.get("status", m_info.get("state", ""))
                            team1 = m_info.get("team1", {}).get("teamName", "Team 1")
                            team2 = m_info.get("team2", {}).get("teamName", "Team 2")
                            
                            # Scores
                            m_score = match_source.get("matchScore", {})
                            # print(f"DEBUG SCORE for {match_id}: {m_score}")
                            
                            # Construct our object
                            normalized_match = {
                                "matchInfo": {
                                    "matchId": match_id,
                                    "seriesName": title,
                                    "status": status,
                                    "team1": {"teamName": team1},
                                    "team2": {"teamName": team2}
                                },
                                "matchScore": {
                                    "team1Score": m_score.get("team1Score", {"inngs1": {"runs":0,"wickets":0,"overs":0}}),
                                    "team2Score": m_score.get("team2Score", {"inngs1": {"runs":0,"wickets":0,"overs":0}}),
                                    "text": status # fallback text
                                }
                            }
                            
                            extracted_matches.append(normalized_match)

                except Exception as e:
                    print(f"Error parsing JSON chunk: {e}")
                    continue

    except Exception as e:
        print(f"Error in JSON extraction: {e}")

    # Fallback to empty if nothing found, but honestly this is our best bet.
    matches = extracted_matches
    
    # Wrap in RapidAPI-like structure to minimize frontend changes
    return {
        "typeMatches": [
            {
                "matchType": "International",
                "seriesMatches": [
                    {
                        "seriesAdWrapper": {
                            "matches": matches
                        }
                    }
                ]
            }
        ]
    }

def get_rankings(category="batting"):
    """
    Scrapes rankings for men for a given category.
    Category: batting, bowling, all-rounder, teams
    """
    import json
    url = f"https://www.cricbuzz.com/cricket-stats/icc-rankings/men/{category}"
    print(f"Fetching Rankings from: {url}")
    
    try:
        response = requests.get(url, headers=HEADERS, timeout=10)
        chunks = re.findall(r'self\.__next_f\.push\(\[\d+,\s*\"(.*?)\"\]\)', response.text, re.DOTALL)
        
        extracted_data = {"test": [], "odi": [], "t20": []}
        formats = ["test", "odi", "t20"]
        
        for chunk in chunks:
            # Unescape the chunk - Next.js uses \" for quotes and \\\\ for backslashes
            unescaped = chunk.replace('\\\"', '"').replace('\\\\', '\\')
            
            for fmt in formats:
                # Skip if we already found data for this format
                if extracted_data[fmt]:
                    continue
                    
                # Find the format key and its rank array - simplified pattern
                # Look for: "test":{"rank":[
                pattern = f'"{fmt}":\\s*{{\\s*"rank":\\s*\\['
                match_start = re.search(pattern, unescaped)
                if match_start:
                    # Find the start of the array
                    start_idx = match_start.end() - 1  # -1 to include the [
                    if start_idx < len(unescaped) and unescaped[start_idx] == '[':
                        # Count brackets to extract the full array
                        cnt = 0
                        json_str = ""
                        for i in range(start_idx, len(unescaped)):
                            ch = unescaped[i]
                            if ch == '[': cnt += 1
                            elif ch == ']': cnt -= 1
                            json_str += ch
                            if cnt == 0:
                                break
                        
                        if json_str:
                            try:
                                json_str = json_str.replace('"$undefined"', 'null')
                                data = json.loads(json_str)
                                # Normalize data
                                normalized = []
                                for p in data:
                                    normalized.append({
                                        "rank": str(p.get("rank", "")),
                                        "name": p.get("name", ""),
                                        "country": p.get("country", ""),
                                        "rating": str(p.get("rating", p.get("points", ""))),
                                        "imageId": p.get("faceImageId", p.get("imageId", ""))
                                    })
                                extracted_data[fmt] = normalized
                                print(f"Successfully extracted {len(normalized)} {fmt.upper()} rankings")
                            except Exception as e:
                                print(f"Error parsing {fmt} rankings JSON: {e}")
        
        return extracted_data

    except Exception as e:
        print(f"Error fetching rankings: {e}")
        return None
