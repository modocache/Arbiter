#ifndef ARBITER_REQUIREMENT_H
#define ARBITER_REQUIREMENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

// forward declarations
struct ArbiterSemanticVersion;
struct ArbiterSelectedVersion;

/**
 * Represents a requirement for a specific version or set of versions.
 */
typedef struct ArbiterRequirement ArbiterRequirement;

/**
 * How strict to be in matching compatible versions.
 */
typedef enum
{
  /**
   * Determine compatibility according to a strict interpretation of SemVer.
   */
  ArbiterRequirementStrictnessStrict,

  /**
   * According to SemVer, technically all 0.y.z releases can break backwards
   * compatibility, meaning that minor and patch versions have to match
   * exactly in order to be "compatible."
   *
   * This looser variant permits newer patch versions, which is probably
   * closer to what the user wants.
   */
  ArbiterRequirementStrictnessAllowVersionZeroPatches,
} ArbiterRequirementStrictness;

/**
 * How suitable a specific version is for a requirement.
 */
typedef enum
{
  /**
   * The version is unsuitable for (does not satisfy) the requirement.
   */
  ArbiterRequirementSuitabilityUnsuitable,

  /**
   * The version is suitable for (satisfies) the requirement.
   */
  ArbiterRequirementSuitabilitySuitable,
  
  /**
   * The version should be considered the best possible choice for satisfying
   * the requirement.
   *
   * This result is unique in that it _overrides other requirements_, which is
   * sometimes desirable (e.g., pinning to a specific named branch or tag
   * instead of a semantic version).
   *
   * If this result is returned multiple times for different versions, it is
   * unspecified which version will be selected.
   */
  ArbiterRequirementSuitabilityBestPossibleChoice,
} ArbiterRequirementSuitability;

/**
 * A predicate used to determine whether the given version suitably satisfies
 * the requirement.
 */
typedef ArbiterRequirementSuitability (*ArbiterRequirementPredicate)(const struct ArbiterSelectedVersion *version, const void *context);

/**
 * Creates a requirement which will match any version.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
ArbiterRequirement *ArbiterCreateRequirementAny (void);

/**
 * Creates a requirement which will match versions not less than the specified
 * version.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
ArbiterRequirement *ArbiterCreateRequirementAtLeast (const struct ArbiterSemanticVersion *version);

/**
 * Creates a requirement which will match versions that are "compatible with"
 * the given version, according to Semantic Versioning rules about backwards
 * compatibility.
 *
 * Exceptions to the SemVer rules can be applied by using a value other than
 * `ArbiterRequirementStrictnessStrict` for `strictness`.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
ArbiterRequirement *ArbiterCreateRequirementCompatibleWith (const struct ArbiterSemanticVersion *version, ArbiterRequirementStrictness strictness);

/**
 * Creates a requirement which will only match the specified version, including
 * any prerelease version and build metadata.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
ArbiterRequirement *ArbiterCreateRequirementExactly (const struct ArbiterSemanticVersion *version);

/**
 * Creates a requirement which will evaluate a custom predicate whenever
 * a specific version is checked against it.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
// TODO: `context` may need memory management
ArbiterRequirement *ArbiterCreateRequirementCustom (ArbiterRequirementPredicate predicate, const void *context);

/**
 * Creates a compound requirement that evaluates each of a list of requirements.
 * All of the requirements must be satisfied for the compound requirement to be
 * satisfied.
 *
 * The objects in the C array can be safely freed after calling this function.
 *
 * The returned requirement must be freed with ArbiterFree().
 */
ArbiterRequirement *ArbiterCreateRequirementCompound (const ArbiterRequirement * const *requirements, size_t count);

/**
 * Determines how well the given requirement is satisfied by the given version.
 */
ArbiterRequirementSuitability ArbiterRequirementSatisfiedBy (const ArbiterRequirement *requirement, const struct ArbiterSelectedVersion *version);

#ifdef __cplusplus
}
#endif

#endif
