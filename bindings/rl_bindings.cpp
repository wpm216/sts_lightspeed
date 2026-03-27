//
// RL-focused Python bindings for sts_lightspeed
// Exposes BattleContext, GameAction, Action, and related types
// needed for step-by-step RL agent interaction.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <sstream>
#include <algorithm>
#include <vector>

#include "combat/BattleContext.h"
#include "combat/CardInstance.h"
#include "combat/CardManager.h"
#include "combat/Monster.h"
#include "combat/MonsterGroup.h"
#include "combat/Player.h"
#include "combat/InputState.h"
#include "game/GameContext.h"
#include "sim/search/Action.h"
#include "sim/search/GameAction.h"
#include "sim/PrintHelpers.h"

namespace py = pybind11;
using namespace sts;

void init_rl_bindings(py::module_ &m) {

    // ============================
    // Potion enum
    // ============================
    py::enum_<Potion>(m, "Potion")
        .value("INVALID", Potion::INVALID)
        .value("EMPTY_POTION_SLOT", Potion::EMPTY_POTION_SLOT)
        .value("AMBROSIA", Potion::AMBROSIA)
        .value("ANCIENT_POTION", Potion::ANCIENT_POTION)
        .value("ATTACK_POTION", Potion::ATTACK_POTION)
        .value("BLESSING_OF_THE_FORGE", Potion::BLESSING_OF_THE_FORGE)
        .value("BLOCK_POTION", Potion::BLOCK_POTION)
        .value("BLOOD_POTION", Potion::BLOOD_POTION)
        .value("BOTTLED_MIRACLE", Potion::BOTTLED_MIRACLE)
        .value("COLORLESS_POTION", Potion::COLORLESS_POTION)
        .value("CULTIST_POTION", Potion::CULTIST_POTION)
        .value("CUNNING_POTION", Potion::CUNNING_POTION)
        .value("DEXTERITY_POTION", Potion::DEXTERITY_POTION)
        .value("DISTILLED_CHAOS", Potion::DISTILLED_CHAOS)
        .value("DUPLICATION_POTION", Potion::DUPLICATION_POTION)
        .value("ELIXIR_POTION", Potion::ELIXIR_POTION)
        .value("ENERGY_POTION", Potion::ENERGY_POTION)
        .value("ENTROPIC_BREW", Potion::ENTROPIC_BREW)
        .value("ESSENCE_OF_DARKNESS", Potion::ESSENCE_OF_DARKNESS)
        .value("ESSENCE_OF_STEEL", Potion::ESSENCE_OF_STEEL)
        .value("EXPLOSIVE_POTION", Potion::EXPLOSIVE_POTION)
        .value("FAIRY_POTION", Potion::FAIRY_POTION)
        .value("FEAR_POTION", Potion::FEAR_POTION)
        .value("FIRE_POTION", Potion::FIRE_POTION)
        .value("FLEX_POTION", Potion::FLEX_POTION)
        .value("FOCUS_POTION", Potion::FOCUS_POTION)
        .value("FRUIT_JUICE", Potion::FRUIT_JUICE)
        .value("GAMBLERS_BREW", Potion::GAMBLERS_BREW)
        .value("GHOST_IN_A_JAR", Potion::GHOST_IN_A_JAR)
        .value("HEART_OF_IRON", Potion::HEART_OF_IRON)
        .value("LIQUID_BRONZE", Potion::LIQUID_BRONZE)
        .value("LIQUID_MEMORIES", Potion::LIQUID_MEMORIES)
        .value("POISON_POTION", Potion::POISON_POTION)
        .value("POTION_OF_CAPACITY", Potion::POTION_OF_CAPACITY)
        .value("POWER_POTION", Potion::POWER_POTION)
        .value("REGEN_POTION", Potion::REGEN_POTION)
        .value("SKILL_POTION", Potion::SKILL_POTION)
        .value("SMOKE_BOMB", Potion::SMOKE_BOMB)
        .value("SNECKO_OIL", Potion::SNECKO_OIL)
        .value("SPEED_POTION", Potion::SPEED_POTION)
        .value("STANCE_POTION", Potion::STANCE_POTION)
        .value("STRENGTH_POTION", Potion::STRENGTH_POTION)
        .value("SWIFT_POTION", Potion::SWIFT_POTION)
        .value("WEAK_POTION", Potion::WEAK_POTION);

    // ============================
    // InputState enum
    // ============================
    py::enum_<InputState>(m, "InputState")
        .value("EXECUTING_ACTIONS", InputState::EXECUTING_ACTIONS)
        .value("PLAYER_NORMAL", InputState::PLAYER_NORMAL)
        .value("CARD_SELECT", InputState::CARD_SELECT)
        .value("CHOOSE_STANCE_ACTION", InputState::CHOOSE_STANCE_ACTION)
        .value("CHOOSE_EXHAUST_POTION_CARDS", InputState::CHOOSE_EXHAUST_POTION_CARDS)
        .value("CHOOSE_GAMBLING_CARDS", InputState::CHOOSE_GAMBLING_CARDS)
        .value("CHOOSE_DISCARD_CARDS", InputState::CHOOSE_DISCARD_CARDS)
        .value("SCRY", InputState::SCRY);

    // ============================
    // Outcome enum (battle-level)
    // ============================
    py::enum_<Outcome>(m, "BattleOutcome")
        .value("UNDECIDED", Outcome::UNDECIDED)
        .value("PLAYER_VICTORY", Outcome::PLAYER_VICTORY)
        .value("PLAYER_LOSS", Outcome::PLAYER_LOSS);

    // ============================
    // MonsterId enum
    // ============================
    py::enum_<MonsterId>(m, "MonsterId")
        .value("INVALID", MonsterId::INVALID)
        .value("ACID_SLIME_L", MonsterId::ACID_SLIME_L)
        .value("ACID_SLIME_M", MonsterId::ACID_SLIME_M)
        .value("ACID_SLIME_S", MonsterId::ACID_SLIME_S)
        .value("AWAKENED_ONE", MonsterId::AWAKENED_ONE)
        .value("BEAR", MonsterId::BEAR)
        .value("BLUE_SLAVER", MonsterId::BLUE_SLAVER)
        .value("BOOK_OF_STABBING", MonsterId::BOOK_OF_STABBING)
        .value("BRONZE_AUTOMATON", MonsterId::BRONZE_AUTOMATON)
        .value("BRONZE_ORB", MonsterId::BRONZE_ORB)
        .value("BYRD", MonsterId::BYRD)
        .value("CENTURION", MonsterId::CENTURION)
        .value("CHOSEN", MonsterId::CHOSEN)
        .value("CORRUPT_HEART", MonsterId::CORRUPT_HEART)
        .value("CULTIST", MonsterId::CULTIST)
        .value("DAGGER", MonsterId::DAGGER)
        .value("DARKLING", MonsterId::DARKLING)
        .value("DECA", MonsterId::DECA)
        .value("DONU", MonsterId::DONU)
        .value("EXPLODER", MonsterId::EXPLODER)
        .value("FAT_GREMLIN", MonsterId::FAT_GREMLIN)
        .value("FUNGI_BEAST", MonsterId::FUNGI_BEAST)
        .value("GIANT_HEAD", MonsterId::GIANT_HEAD)
        .value("GREEN_LOUSE", MonsterId::GREEN_LOUSE)
        .value("GREMLIN_LEADER", MonsterId::GREMLIN_LEADER)
        .value("GREMLIN_NOB", MonsterId::GREMLIN_NOB)
        .value("GREMLIN_WIZARD", MonsterId::GREMLIN_WIZARD)
        .value("HEXAGHOST", MonsterId::HEXAGHOST)
        .value("JAW_WORM", MonsterId::JAW_WORM)
        .value("LAGAVULIN", MonsterId::LAGAVULIN)
        .value("LOOTER", MonsterId::LOOTER)
        .value("MAD_GREMLIN", MonsterId::MAD_GREMLIN)
        .value("MUGGER", MonsterId::MUGGER)
        .value("MYSTIC", MonsterId::MYSTIC)
        .value("NEMESIS", MonsterId::NEMESIS)
        .value("ORB_WALKER", MonsterId::ORB_WALKER)
        .value("POINTY", MonsterId::POINTY)
        .value("RED_LOUSE", MonsterId::RED_LOUSE)
        .value("RED_SLAVER", MonsterId::RED_SLAVER)
        .value("REPTOMANCER", MonsterId::REPTOMANCER)
        .value("REPULSOR", MonsterId::REPULSOR)
        .value("ROMEO", MonsterId::ROMEO)
        .value("SENTRY", MonsterId::SENTRY)
        .value("SHELLED_PARASITE", MonsterId::SHELLED_PARASITE)
        .value("SHIELD_GREMLIN", MonsterId::SHIELD_GREMLIN)
        .value("SLIME_BOSS", MonsterId::SLIME_BOSS)
        .value("SNAKE_PLANT", MonsterId::SNAKE_PLANT)
        .value("SNEAKY_GREMLIN", MonsterId::SNEAKY_GREMLIN)
        .value("SNECKO", MonsterId::SNECKO)
        .value("SPHERIC_GUARDIAN", MonsterId::SPHERIC_GUARDIAN)
        .value("SPIKER", MonsterId::SPIKER)
        .value("SPIKE_SLIME_L", MonsterId::SPIKE_SLIME_L)
        .value("SPIKE_SLIME_M", MonsterId::SPIKE_SLIME_M)
        .value("SPIKE_SLIME_S", MonsterId::SPIKE_SLIME_S)
        .value("SPIRE_GROWTH", MonsterId::SPIRE_GROWTH)
        .value("SPIRE_SHIELD", MonsterId::SPIRE_SHIELD)
        .value("SPIRE_SPEAR", MonsterId::SPIRE_SPEAR)
        .value("TASKMASTER", MonsterId::TASKMASTER)
        .value("THE_CHAMP", MonsterId::THE_CHAMP)
        .value("THE_COLLECTOR", MonsterId::THE_COLLECTOR)
        .value("THE_GUARDIAN", MonsterId::THE_GUARDIAN)
        .value("THE_MAW", MonsterId::THE_MAW)
        .value("TIME_EATER", MonsterId::TIME_EATER)
        .value("TORCH_HEAD", MonsterId::TORCH_HEAD)
        .value("TRANSIENT", MonsterId::TRANSIENT)
        .value("WRITHING_MASS", MonsterId::WRITHING_MASS);

    // ============================
    // CardInstance (combat card)
    // ============================
    py::class_<CardInstance>(m, "CardInstance")
        .def_readonly("id", &CardInstance::id)
        .def_readonly("unique_id", &CardInstance::uniqueId)
        .def_readonly("cost", &CardInstance::cost)
        .def_readonly("cost_for_turn", &CardInstance::costForTurn)
        .def_readonly("upgraded", &CardInstance::upgraded)
        .def_readonly("free_to_play_once", &CardInstance::freeToPlayOnce)
        .def_readonly("retain", &CardInstance::retain)
        .def("get_type", &CardInstance::getType)
        .def("get_name", &CardInstance::getName)
        .def("requires_target", &CardInstance::requiresTarget)
        .def("is_x_cost", &CardInstance::isXCost)
        .def("does_exhaust", &CardInstance::doesExhaust)
        .def("__repr__", [](const CardInstance &c) {
            std::string s("<CardInstance ");
            s += c.getName();
            if (c.isUpgraded()) s += '+';
            s += " cost=";
            s += std::to_string(c.costForTurn);
            s += ">";
            return s;
        });

    // ============================
    // Monster
    // ============================
    py::class_<Monster>(m, "Monster")
        .def_readonly("id", &Monster::id)
        .def_readonly("cur_hp", &Monster::curHp)
        .def_readonly("max_hp", &Monster::maxHp)
        .def_readonly("block", &Monster::block)
        .def_readonly("strength", &Monster::strength)
        .def_readonly("vulnerable", &Monster::vulnerable)
        .def_readonly("weak", &Monster::weak)
        .def_readonly("poison", &Monster::poison)
        .def_readonly("artifact", &Monster::artifact)
        .def_readonly("metallicize", &Monster::metallicize)
        .def_readonly("plated_armor", &Monster::platedArmor)
        .def_readonly("regen", &Monster::regen)
        .def("is_alive", &Monster::isAlive)
        .def("is_targetable", &Monster::isTargetable)
        .def("is_half_dead", &Monster::isHalfDead)
        .def("is_escaping", &Monster::isEscaping)
        .def("is_attacking", &Monster::isAttacking)
        .def("get_name", &Monster::getName)
        .def("get_move_damage", [](const Monster &m, const BattleContext &bc) {
            return m.getMoveBaseDamage(bc);
        }, "get base damage info for monster's current move")
        .def_property_readonly("move_id", [](const Monster &m) {
            return m.moveHistory[0];
        })
        .def("__repr__", [](const Monster &m) {
            std::string s("<Monster ");
            s += m.getName();
            s += " hp=";
            s += std::to_string(m.curHp);
            s += "/";
            s += std::to_string(m.maxHp);
            if (!m.isAlive()) s += " DEAD";
            s += ">";
            return s;
        });

    // DamageInfo
    py::class_<DamageInfo>(m, "DamageInfo")
        .def_readonly("damage", &DamageInfo::damage)
        .def_readonly("attack_count", &DamageInfo::attackCount);

    // ============================
    // Player (combat)
    // ============================
    py::class_<Player>(m, "CombatPlayer")
        .def_readonly("cur_hp", &Player::curHp)
        .def_readonly("max_hp", &Player::maxHp)
        .def_readonly("energy", &Player::energy)
        .def_readonly("block", &Player::block)
        .def_readonly("strength", &Player::strength)
        .def_readonly("dexterity", &Player::dexterity)
        .def_readonly("focus", &Player::focus)
        .def_readonly("artifact", &Player::artifact)
        .def_readonly("energy_per_turn", &Player::energyPerTurn)
        .def_readonly("card_draw_per_turn", &Player::cardDrawPerTurn)
        .def_readonly("cards_played_this_turn", &Player::cardsPlayedThisTurn)
        .def_readonly("attacks_played_this_turn", &Player::attacksPlayedThisTurn)
        .def("get_status", [](const Player &p, PlayerStatus s) {
            auto it = p.statusMap.find(s);
            if (it != p.statusMap.end()) return (int)it->second;
            return 0;
        }, "get the value of a player status effect")
        .def("has_status", [](const Player &p, PlayerStatus s) {
            return p.statusMap.find(s) != p.statusMap.end() && p.statusMap.at(s) != 0;
        }, "check if player has a status effect");

    // PlayerStatus enum (partial - key statuses)
    py::enum_<PlayerStatus>(m, "PlayerStatus")
        .value("VULNERABLE", PlayerStatus::VULNERABLE)
        .value("WEAK", PlayerStatus::WEAK)
        .value("FRAIL", PlayerStatus::FRAIL)
        .value("STRENGTH", PlayerStatus::STRENGTH)
        .value("DEXTERITY", PlayerStatus::DEXTERITY)
        .value("INTANGIBLE", PlayerStatus::INTANGIBLE)
        .value("DRAW_REDUCTION", PlayerStatus::DRAW_REDUCTION)
        .value("ENTANGLED", PlayerStatus::ENTANGLED)
        .value("NO_DRAW", PlayerStatus::NO_DRAW)
        .value("BARRICADE", PlayerStatus::BARRICADE)
        .value("CORRUPTION", PlayerStatus::CORRUPTION)
        .value("DOUBLE_TAP", PlayerStatus::DOUBLE_TAP)
        .value("BUFFER", PlayerStatus::BUFFER)
        .value("METALLICIZE", PlayerStatus::METALLICIZE)
        .value("PLATED_ARMOR", PlayerStatus::PLATED_ARMOR)
        .value("THORNS", PlayerStatus::THORNS)
        .value("AFTER_IMAGE", PlayerStatus::AFTER_IMAGE)
        .value("NOXIOUS_FUMES", PlayerStatus::NOXIOUS_FUMES)
        .value("BLUR", PlayerStatus::BLUR)
        .value("RAGE", PlayerStatus::RAGE)
        .value("COMBUST", PlayerStatus::COMBUST)
        .value("RUPTURE", PlayerStatus::RUPTURE)
        .value("BRUTALITY", PlayerStatus::BRUTALITY)
        .value("FEEL_NO_PAIN", PlayerStatus::FEEL_NO_PAIN)
        .value("DARK_EMBRACE", PlayerStatus::DARK_EMBRACE)
        .value("EVOLVE", PlayerStatus::EVOLVE)
        .value("FIRE_BREATHING", PlayerStatus::FIRE_BREATHING)
        .value("JUGGERNAUT", PlayerStatus::JUGGERNAUT)
        .value("FLAME_BARRIER", PlayerStatus::FLAME_BARRIER)
        .value("SADISTIC", PlayerStatus::SADISTIC)
        .value("PEN_NIB", PlayerStatus::PEN_NIB)
        .value("CONFUSED", PlayerStatus::CONFUSED)
        .value("HEX", PlayerStatus::HEX);

    // ============================
    // Combat Action (search::Action)
    // ============================
    py::enum_<search::ActionType>(m, "CombatActionType")
        .value("CARD", search::ActionType::CARD)
        .value("POTION", search::ActionType::POTION)
        .value("SINGLE_CARD_SELECT", search::ActionType::SINGLE_CARD_SELECT)
        .value("MULTI_CARD_SELECT", search::ActionType::MULTI_CARD_SELECT)
        .value("END_TURN", search::ActionType::END_TURN);

    py::class_<search::Action>(m, "CombatAction")
        .def(py::init<search::ActionType>())
        .def(py::init<search::ActionType, int>())
        .def(py::init<search::ActionType, int, int>())
        .def("get_action_type", &search::Action::getActionType)
        .def("get_source_idx", &search::Action::getSourceIdx)
        .def("get_target_idx", &search::Action::getTargetIdx)
        .def("is_valid", &search::Action::isValidAction)
        .def("execute", &search::Action::execute)
        .def("__eq__", &search::Action::operator==)
        .def("__repr__", [](const search::Action &a) {
            std::string s("<CombatAction type=");
            switch(a.getActionType()) {
                case search::ActionType::CARD: s += "CARD src=" + std::to_string(a.getSourceIdx()) + " tgt=" + std::to_string(a.getTargetIdx()); break;
                case search::ActionType::POTION: s += "POTION src=" + std::to_string(a.getSourceIdx()) + " tgt=" + std::to_string(a.getTargetIdx()); break;
                case search::ActionType::SINGLE_CARD_SELECT: s += "SINGLE_CARD_SELECT idx=" + std::to_string(a.getSelectIdx()); break;
                case search::ActionType::MULTI_CARD_SELECT: s += "MULTI_CARD_SELECT"; break;
                case search::ActionType::END_TURN: s += "END_TURN"; break;
            }
            s += ">";
            return s;
        });

    // ============================
    // GameAction (non-combat action)
    // ============================
    py::class_<search::GameAction>(m, "GameAction")
        .def(py::init<int, int>(), py::arg("idx1"), py::arg("idx2") = 0)
        .def("is_valid", &search::GameAction::isValidAction)
        .def("execute", &search::GameAction::execute)
        .def("get_idx1", &search::GameAction::getIdx1)
        .def("get_idx2", &search::GameAction::getIdx2)
        .def_static("get_all_actions", &search::GameAction::getAllActionsInState,
            "get all valid actions for the current non-combat game state")
        .def("__repr__", [](const search::GameAction &a, const GameContext &gc) {
            std::ostringstream oss;
            a.printDesc(oss, gc);
            return "<GameAction " + oss.str() + ">";
        });

    // ============================
    // BattleContext
    // ============================
    py::class_<BattleContext>(m, "BattleContext")
        .def(py::init<>())
        .def("init", py::overload_cast<const GameContext&>(&BattleContext::init),
            "initialize battle from a GameContext that is in BATTLE screen state")
        .def("exit_battle", &BattleContext::exitBattle,
            "transfer battle results back to the GameContext")

        // State
        .def_readonly("outcome", &BattleContext::outcome)
        .def_readonly("input_state", &BattleContext::inputState)
        .def_readonly("turn", &BattleContext::turn)
        .def_readonly("is_battle_over", &BattleContext::isBattleOver)
        .def_readonly("ascension", &BattleContext::ascension)
        .def_readonly("encounter", &BattleContext::encounter)

        // Player
        .def_readonly("player", &BattleContext::player)

        // Potions in combat
        .def_readonly("potion_count", &BattleContext::potionCount)
        .def_readonly("potion_capacity", &BattleContext::potionCapacity)
        .def_property_readonly("potions", [](const BattleContext &bc) {
            std::vector<Potion> pots(bc.potions.begin(), bc.potions.begin() + bc.potionCapacity);
            return pots;
        })

        // Monsters
        .def_property_readonly("monsters", [](const BattleContext &bc) {
            std::vector<Monster> ms;
            for (int i = 0; i < bc.monsters.monsterCount; i++) {
                ms.push_back(bc.monsters.arr[i]);
            }
            return ms;
        })
        .def_property_readonly("monster_count", [](const BattleContext &bc) {
            return bc.monsters.monsterCount;
        })
        .def_property_readonly("monsters_alive", [](const BattleContext &bc) {
            return bc.monsters.monstersAlive;
        })

        // Cards
        .def_property_readonly("hand", [](const BattleContext &bc) {
            return std::vector<CardInstance>(bc.cards.hand.begin(), bc.cards.hand.begin() + bc.cards.cardsInHand);
        })
        .def_property_readonly("hand_size", [](const BattleContext &bc) {
            return bc.cards.cardsInHand;
        })
        .def_property_readonly("draw_pile", [](const BattleContext &bc) {
            return std::vector<CardInstance>(bc.cards.drawPile.begin(), bc.cards.drawPile.end());
        })
        .def_property_readonly("discard_pile", [](const BattleContext &bc) {
            return std::vector<CardInstance>(bc.cards.discardPile.begin(), bc.cards.discardPile.end());
        })
        .def_property_readonly("exhaust_pile", [](const BattleContext &bc) {
            return std::vector<CardInstance>(bc.cards.exhaustPile.begin(), bc.cards.exhaustPile.end());
        })

        // Is the battle actually over (outcome decided)?
        .def("is_over", [](const BattleContext &bc) {
            return bc.outcome != Outcome::UNDECIDED;
        }, "check if battle outcome has been decided")

        // Get legal combat actions
        .def("get_legal_actions", [](const BattleContext &bc) -> std::vector<search::Action> {
            if (bc.outcome != Outcome::UNDECIDED) {
                return {};
            }
            if (bc.inputState == InputState::PLAYER_NORMAL) {
                std::vector<search::Action> actions;
                // End turn is always legal
                actions.push_back(search::Action(search::ActionType::END_TURN));

                // Enumerate playable cards
                for (int i = 0; i < bc.cards.cardsInHand; i++) {
                    const auto &card = bc.cards.hand[i];
                    if (card.requiresTarget()) {
                        for (int t = 0; t < bc.monsters.monsterCount; t++) {
                            if (bc.monsters.arr[t].isTargetable()) {
                                search::Action a(search::ActionType::CARD, i, t);
                                if (a.isValidAction(bc)) {
                                    actions.push_back(a);
                                }
                            }
                        }
                    } else {
                        search::Action a(search::ActionType::CARD, i, 0);
                        if (a.isValidAction(bc)) {
                            actions.push_back(a);
                        }
                    }
                }

                // Enumerate potions
                for (int i = 0; i < bc.potionCapacity; i++) {
                    if (bc.potions[i] != Potion::EMPTY_POTION_SLOT) {
                        // Try targeting each monster
                        for (int t = 0; t < bc.monsters.monsterCount; t++) {
                            if (bc.monsters.arr[t].isTargetable()) {
                                search::Action a(search::ActionType::POTION, i, t);
                                if (a.isValidAction(bc)) {
                                    actions.push_back(a);
                                }
                            }
                        }
                        // Also try with target 0 for non-targeted potions
                        search::Action a(search::ActionType::POTION, i, 0);
                        if (a.isValidAction(bc)) {
                            // Deduplicate: only add if not already present
                            bool found = false;
                            for (const auto &existing : actions) {
                                if (existing == a) { found = true; break; }
                            }
                            if (!found) actions.push_back(a);
                        }
                    }
                }

                return actions;
            } else if (bc.inputState == InputState::CARD_SELECT) {
                return search::Action::enumerateCardSelectActions(bc);
            }
            return {};
        }, "enumerate all legal actions in the current battle state")

        // Step: execute an action and advance to next decision point
        .def("step", [](BattleContext &bc, const search::Action &action) {
            action.execute(bc);
            // After executing, the engine may need to process the action queue
            // The execute() call handles this internally
        }, "execute a combat action")

        .def("__repr__", [](const BattleContext &bc) {
            std::ostringstream oss;
            oss << bc;
            return "<BattleContext " + oss.str() + ">";
        });

    // ============================
    // Extend GameContext with action interface
    // ============================
    // We add these as module-level functions since GameContext is already bound
    m.def("get_legal_game_actions", [](const GameContext &gc) {
        return search::GameAction::getAllActionsInState(gc);
    }, "get all legal non-combat actions for the current game state");

    m.def("execute_game_action", [](GameContext &gc, const search::GameAction &action) {
        action.execute(gc);
    }, "execute a non-combat action on the game context");

    m.def("create_battle_context", [](const GameContext &gc) {
        BattleContext bc;
        bc.init(gc);
        return bc;
    }, "create a BattleContext from a GameContext in BATTLE screen state");

    m.def("exit_battle", [](const BattleContext &bc, GameContext &gc) {
        bc.exitBattle(gc);
    }, "transfer battle results back to the game context");
}
