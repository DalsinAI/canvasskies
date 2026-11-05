#ifndef CANVAS_CAMPAIGN_H
#define CANVAS_CAMPAIGN_H

typedef enum {
    MISSION_SIDE,
    MISSION_TOP_DOWN,
    MISSION_DOGFIGHT_3D,
    MISSION_GUNNER_3D
} MissionMode;

/* A few sorties step outside the usual pattern for one day of the war. */
typedef enum {
    SPECIAL_NONE,
    SPECIAL_TRUCE,   /* Christmas 1914: nobody fires, one pudding to drop */
    SPECIAL_AIRSHIP, /* Home Defence at night against a raiding airship */
    SPECIAL_GOTHA    /* Daylight bombers over the Thames */
} MissionSpecial;

typedef struct {
    MissionMode mode;
    const char *title;
    const char *log;
    int special;
} CampaignMission;

#define MISSIONS_PER_CHAPTER 12

static const char *allied_pilot_names[3] = {
    "ECCLES", "GRIDPIPE THYNE", "BLUEBOTTLE"
};

/* Neddy Seagoon's continuous first-person campaign. The modes rotate
 * four times per chapter so no period of the war belongs to one game style. */
/* Most missions are ordinary: their special is left as SPECIAL_NONE. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
static const CampaignMission campaign_missions[8][MISSIONS_PER_CHAPTER] = {
    {
        {MISSION_SIDE, "A FIELD IN FRANCE", "I landed in a cow field. Tom called it our aerodrome."},
        {MISSION_TOP_DOWN, "THE FIRST MESSAGE", "The general's note rode in my coat while rifles cracked below."},
        {MISSION_GUNNER_3D, "A STRANGER ABOVE", "A German flier waved. Neither of us had a gun."},
        {MISSION_TOP_DOWN, "COLUMNS ON THE ROAD", "Tom counted grey columns; I held the machine steady."},
        {MISSION_SIDE, "THE MARNE BRIDGE", "We found the bridge intact and carried the news west."},
        {MISSION_DOGFIGHT_3D, "HORSES IN THE SKY", "We circled an enemy machine like cavalrymen without sabres."},
        {MISSION_TOP_DOWN, "THE EMPTY FLANK", "The gap on Tom's map changed the army's march."},
        {MISSION_SIDE, "RAIN OVER MEAUX", "The engine coughed all the way home. Alfie never complained."},
        {MISSION_GUNNER_3D, "A FACE IN THE WIND", "For one moment I saw the other pilot's eyes."},
        {MISSION_TOP_DOWN, "TURN OF THE ARMIES", "The roads bent north. The retreat had become a pursuit."},
        {MISSION_DOGFIGHT_3D, "EYES ABOVE THE MARNE", "Tom said the aeroplane had earned its place in the war."},
        {MISSION_TOP_DOWN, "THE SILENT FRONT", "Nobody fired all day. Below us men kicked a ball where the wire had been.", SPECIAL_TRUCE}
    },
    {
        {MISSION_SIDE, "WINTER PHOTOGRAPHS", "The camera froze, so Tom drew the trenches by hand."},
        {MISSION_TOP_DOWN, "THE WIRELESS SET", "A great box of valves made our fragile machine even heavier."},
        {MISSION_GUNNER_3D, "PISTOLS AT ALTITUDE", "We traded pistol shots with an aviator above Ypres."},
        {MISSION_TOP_DOWN, "GAS CLOUD", "A green line moved across the earth. Tom stopped talking."},
        {MISSION_SIDE, "ALFIE'S ENGINE", "Alfie gave us six more horsepower and asked for all six back."},
        {MISSION_DOGFIGHT_3D, "THE INTERRUPTER", "The Fokker fired through its propeller. The sky had changed."},
        {MISSION_TOP_DOWN, "LOST BATTERY", "We found the hidden guns by their tracks in the mud."},
        {MISSION_SIDE, "NO MAN'S LAND", "We dropped water to men who could not reach either trench."},
        {MISSION_GUNNER_3D, "TOM'S LEWIS GUN", "Tom stood in the slipstream and drove the Fokker away."},
        {MISSION_TOP_DOWN, "THE SALIENT", "Every line on the map was a place somebody had died."},
        {MISSION_SIDE, "THE BURNING HANGAR", "We saved two machines. The third took our letters with it."},
        {MISSION_DOGFIGHT_3D, "THE WAR LEARNS TO FLY", "Another victory, and still I could not celebrate it."}
    },
    {
        {MISSION_SIDE, "BEFORE THE BARRAGE", "The Somme looked peaceful until every gun spoke at once."},
        {MISSION_TOP_DOWN, "CUT THE BALLOON LINE", "We flew low enough to see men pointing up from the trench."},
        {MISSION_GUNNER_3D, "ABOVE ALBERT", "Three enemy scouts came out of the sun."},
        {MISSION_TOP_DOWN, "ZERO HOUR", "Tom marked the falling shells while the earth disappeared."},
        {MISSION_SIDE, "THE FIRST TRENCH", "Our bombs found the gun pit; the infantry found the wire."},
        {MISSION_DOGFIGHT_3D, "ECCLES' WING", "Eccles brought me home with half his left wing gone."},
        {MISSION_TOP_DOWN, "A RED ROCKET", "The signal rocket rose from men cut off beyond the line."},
        {MISSION_SIDE, "SUPPLIES AT POZIERES", "We dropped ammunition into a village made of brick dust."},
        {MISSION_GUNNER_3D, "THE EMPTY SEAT", "Tom's replacement sat quietly where Tom had been wounded."},
        {MISSION_TOP_DOWN, "THE NEW TRENCH", "By morning both armies had dug new lines into the map."},
        {MISSION_DOGFIGHT_3D, "HOME DEFENCE", "They sent us home to hunt the airships. Essex watched the sky burn.", SPECIAL_AIRSHIP},
        {MISSION_DOGFIGHT_3D, "OVER THE SOMME", "We won a mile below and lost half the squadron above."}
    },
    {
        {MISSION_SIDE, "ARRAS AT DAWN", "The new camera saw everything; the enemy saw us too."},
        {MISSION_TOP_DOWN, "THE FORWARD STRIP", "Alfie worked in a tent while shells walked toward the field."},
        {MISSION_GUNNER_3D, "THE CIRCUS", "Their painted scouts arrived above us in perfect formation."},
        {MISSION_TOP_DOWN, "MISSING PATROL", "I found their wrecks before I found the objective."},
        {MISSION_SIDE, "VIMY RIDGE", "We silenced a gun that had held the ridge since winter."},
        {MISSION_DOGFIGHT_3D, "SIX AGAINST TWO", "Bluebottle laughed over the signal lamp. I did not."},
        {MISSION_TOP_DOWN, "THE PHOTOGRAPH", "One clear plate was worth three lost machines."},
        {MISSION_SIDE, "TOM RETURNS", "Tom climbed back into the rear cockpit with a stiff left arm."},
        {MISSION_GUNNER_3D, "CAPTAIN MERCER", "The promotion came with a desk I refused to use."},
        {MISSION_TOP_DOWN, "EMPTY CHAIRS", "I wrote four letters home before breakfast."},
        {MISSION_DOGFIGHT_3D, "THE GOTHAS", "Big grey bombers crossed the Thames in daylight. London will not forget.", SPECIAL_GOTHA},
        {MISSION_DOGFIGHT_3D, "BLOODY APRIL", "Spring ended. Few of the faces from March remained."}
    },
    {
        {MISSION_SIDE, "TANKS IN THE MIST", "Iron shapes moved below us toward Cambrai."},
        {MISSION_TOP_DOWN, "THE GUN LINE", "We attacked the field guns, not the machines crawling past them."},
        {MISSION_GUNNER_3D, "HIGH COVER", "Our job was to keep the sky off the tanks' backs."},
        {MISSION_TOP_DOWN, "THE CANAL MAP", "Tom found the crossing beneath camouflage nets."},
        {MISSION_SIDE, "ROAD TO MASNIERES", "A broken bridge turned the whole column south."},
        {MISSION_DOGFIGHT_3D, "THE NEW ALBATROS", "The new enemy scout climbed as if the war were easy."},
        {MISSION_TOP_DOWN, "COUNTERATTACK", "Grey reserves filled every road we had photographed empty."},
        {MISSION_SIDE, "THE STRANDED CREW", "We held the guns off a tank crew running for home."},
        {MISSION_GUNNER_3D, "THYNE'S LAST TURN", "Gridpipe Thyne waved once, turned east, and vanished in cloud."},
        {MISSION_TOP_DOWN, "GROUND LOST AGAIN", "The pencils on the map moved back to their old lines."},
        {MISSION_SIDE, "BRING ALFIE HOME", "Alfie rode out on a lorry and we found him after dark."},
        {MISSION_DOGFIGHT_3D, "THE IRON ROAD", "Cambrai taught us that no invention ends a war."}
    },
    {
        {MISSION_SIDE, "FOG ON THE SOMME", "The enemy offensive appeared under us all at once."},
        {MISSION_TOP_DOWN, "BURN THE STORES", "We destroyed our own fuel before the retreat."},
        {MISSION_GUNNER_3D, "WESTWARD", "Every machine in the sky seemed to be flying west."},
        {MISSION_TOP_DOWN, "THE BROKEN LINE", "There was no front, only columns and gaps."},
        {MISSION_SIDE, "THE FIELD HOSPITAL", "We drew the attackers away from Clara's hospital train."},
        {MISSION_DOGFIGHT_3D, "TOM'S FLARE", "Tom's flare brought three lost pilots out of the cloud."},
        {MISSION_TOP_DOWN, "AMIENS ROAD", "Refugees and gun teams shared the same narrow road."},
        {MISSION_SIDE, "THE LAST CRATE", "Alfie built one flying machine from three broken ones."},
        {MISSION_GUNNER_3D, "HOLD THE SKY", "We fought above a field that no longer had a name."},
        {MISSION_TOP_DOWN, "THE LINE HOLDS", "At last the red pencil stopped moving west."},
        {MISSION_SIDE, "COUNTERSTROKE", "For the first time that spring, our bombs fell east of us."},
        {MISSION_DOGFIGHT_3D, "THE SPRING STORM", "We survived the storm, but none of us called it victory."}
    },
    {
        {MISSION_SIDE, "AMIENS AT FOUR", "The attack began without the old warning barrage."},
        {MISSION_TOP_DOWN, "BREAK THE BATTERY", "We opened the road before the tanks reached it."},
        {MISSION_GUNNER_3D, "SKY OF MANY FLAGS", "British, French, and American wings filled the dawn."},
        {MISSION_TOP_DOWN, "THE MOVING FRONT", "Headquarters needed new maps twice in one day."},
        {MISSION_SIDE, "THE RAILHEAD", "Our bombs caught the gun train before it could unload."},
        {MISSION_DOGFIGHT_3D, "THE VETERAN", "The young pilots now watched my hands before takeoff."},
        {MISSION_TOP_DOWN, "CANAL DU NORD", "Tom found a dry approach through the flooded ground."},
        {MISSION_SIDE, "OPEN COUNTRY", "After years of trenches, the fields seemed impossibly wide."},
        {MISSION_GUNNER_3D, "THE LAST ACE", "We fought a red scout who would not leave his men."},
        {MISSION_TOP_DOWN, "RETREAT EAST", "Abandoned guns marked the enemy's road home."},
        {MISSION_SIDE, "THE BELLS OF CAMBRAI", "Church bells rang below us as the town was freed."},
        {MISSION_DOGFIGHT_3D, "THE HUNDRED DAYS", "The end felt close enough to fear."}
    },
    {
        {MISSION_SIDE, "NOVEMBER MAP", "The front moved east through towns whose bells still worked."},
        {MISSION_TOP_DOWN, "A LETTER FROM CLARA", "Clara wrote that an armistice was being discussed."},
        {MISSION_GUNNER_3D, "NO EASY TRUCE", "A patrol attacked as if peace were an insult."},
        {MISSION_TOP_DOWN, "THE WHITE TRAIN", "Tom spotted the negotiators' train in the forest."},
        {MISSION_SIDE, "LAST BATTERY", "We silenced a gun that fired on a withdrawing column."},
        {MISSION_DOGFIGHT_3D, "THE OLD ENEMY", "The pilot waved, as one had in 1914, and turned away."},
        {MISSION_TOP_DOWN, "ORDERS FOR ELEVEN", "The message said the firing would end at eleven."},
        {MISSION_SIDE, "TEN THIRTY", "We carried the order to a battery that had not heard."},
        {MISSION_GUNNER_3D, "TEN FORTY-FIVE", "We flew the line without firing. Every minute mattered."},
        {MISSION_TOP_DOWN, "TEN FIFTY-FIVE", "Men climbed from the trenches and simply stood there."},
        {MISSION_SIDE, "THE FINAL MESSAGE", "Tom dropped the order beside a gun still firing."},
        {MISSION_DOGFIGHT_3D, "THE ELEVENTH HOUR", "At eleven the earth went quiet. I turned for home."}
    }
};
#pragma GCC diagnostic pop

/* The duty as the squadron's orders book would name it, not the view the
 * game draws it in. */
static const char *mission_mode_name(MissionMode mode)
{
    switch (mode) {
    case MISSION_TOP_DOWN: return "RECONNAISSANCE";
    case MISSION_DOGFIGHT_3D: return "OFFENSIVE PATROL";
    case MISSION_GUNNER_3D: return "OBSERVER'S GUN";
    default: return "CONTACT PATROL";
    }
}

#endif
