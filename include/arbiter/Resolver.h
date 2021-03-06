#ifndef ARBITER_RESOLVER_H
#define ARBITER_RESOLVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <arbiter/Value.h>

#include <stdbool.h>

// forward declarations
struct ArbiterDependencyList;
struct ArbiterProjectIdentifier;
struct ArbiterResolvedDependencyGraph;
struct ArbiterSelectedVersion;
struct ArbiterSelectedVersionList;

/**
 * A dependency resolver which contains context about how to evaluate the
 * dependency graph.
 */
typedef struct ArbiterResolver ArbiterResolver;

/**
 * User-provided behaviors for how dependency resolution should work.
 */
typedef struct
{
  /**
   * Requests the list of dependencies needed by a specific version of
   * a project.
   *
   * Returns a dependency list, or NULL if an error occurs. Arbiter will be
   * responsible for freeing the returned dependency list object. If returning
   * NULL, `error` may be set to a string describing the error which occurred,
   * in which case Arbiter will be responsible for freeing the string.
   */
  struct ArbiterDependencyList *(*createDependencyList)(const ArbiterResolver *resolver, const struct ArbiterProjectIdentifier *project, const struct ArbiterSelectedVersion *selectedVersion, char **error);

  /**
   * Requests the list of versions available for a given project.
   *
   * Returns a version list, or NULL if an error occurs. Arbiter will be
   * responsible for freeing the returned version list object. If returning
   * NULL, `error` may be set to a string describing the error which occurred,
   * in which case Arbiter will be responsible for freeing the string.
   */
  struct ArbiterSelectedVersionList *(*createAvailableVersionsList)(const ArbiterResolver *resolver, const struct ArbiterProjectIdentifier *project, char **error);

  /**
   * Requests the selected version which corresponds to the given metadata.
   *
   * This behavior can be used to implement lookup of versions which are not
   * known in advance (i.e., those which would not appear in the result of
   * `createAvailableVersionsList`). For example, it is impractical to list all
   * commit hashes from a version control system, but they could be looked up by
   * hash here.
   *
   * This behavior is optional, and may be set to NULL if unsupported or
   * unnecessary.
   *
   * Returns a selected version, or NULL if one corresponding to the metadata
   * could not be found.
   */
  struct ArbiterSelectedVersion *(*createSelectedVersionForMetadata)(const ArbiterResolver *resolver, const void *metadata);
} ArbiterResolverBehaviors;

/**
 * Creates a dependency resolver, implemented using the given behaviors, which
 * will attempt to pick compatible versions of all dependencies in
 * `dependencyList` and transitive dependencies thereof.
 *
 * The returned dependency resolver must be freed with ArbiterFree().
 */
ArbiterResolver *ArbiterCreateResolver (ArbiterResolverBehaviors behaviors, const struct ArbiterDependencyList *dependencyList, const void *context);

/**
 * Returns any context data which was provided to ArbiterCreateResolver().
 *
 * The returned pointer is only guaranteed to remain valid for the current
 * scope.
 */
const void *ArbiterResolverContext (const ArbiterResolver *resolver);

/**
 * Attempts to resolve all dependencies.
 *
 * Returns the graph of resolved dependencies, or NULL if an error occurred. The
 * caller is responsible for freeing the returned graph. If NULL is returned and
 * `error` is not NULL, it may be set to a string describing the error, which
 * the caller is responsible for freeing.
 */
struct ArbiterResolvedDependencyGraph *ArbiterResolverCreateResolvedDependencyGraph (ArbiterResolver *resolver, char **error);

#ifdef __cplusplus
}
#endif

#endif
